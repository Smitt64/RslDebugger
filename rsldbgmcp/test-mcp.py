# test-mcp.py - simple rsldbgmcp.exe check without an MCP client.
#
# Level 1 (smoke): initialize + tools/list + rsl_sessions.
# Level 2 (full):  if a live session is found (or --uri given) -
#   connect -> wait_event -> stack/locals -> step_over -> wait_event -> disconnect.
#
# Usage:
#   python test-mcp.py
#   python test-mcp.py --uri rsldbg://127.0.0.1:63873
#
# For the full scenario: client_mode = agent in the host app rsldbg.ini,
# then run debugbreak (the session URI goes to the log and clipboard).

import argparse
import json
import os
import subprocess
import sys
import tempfile

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
DEFAULT_EXE = os.path.join(SCRIPT_DIR, "..", "bin", "rsldbgmcp.exe")
DEFAULT_QTENV = r"D:\Qt\5.15.2\msvc2019\bin\qtenv2.bat"
ERR_LOG = os.path.join(tempfile.gettempdir(), "rsldbgmcp-stderr.log")


def load_qt_env(qtenv_path):
    """Import environment variables from qtenv2.bat (Qt DLL dirs in PATH)."""
    env = dict(os.environ)
    if not os.path.exists(qtenv_path):
        print(f"WARNING: qtenv2.bat not found: {qtenv_path}", file=sys.stderr)
        return env
    out = subprocess.run(
        f'"{qtenv_path}" >nul 2>&1 && set',
        shell=True, capture_output=True, text=True,
    ).stdout
    for line in out.splitlines():
        if "=" in line:
            name, _, value = line.partition("=")
            env[name] = value
    return env


def invoke_mcp(requests, exe, env):
    """Run one batch of JSON-RPC requests; stderr (logs) goes to a temp file.

    All requests are written upfront and stdin is closed right after - the
    server must keep processing the queue and quit only when it drains.
    """
    proc = subprocess.Popen(
        [exe],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        env=env,
        text=True,
        encoding="utf-8",
        errors="replace",
    )
    payload = "".join(r + "\n" for r in requests)
    out, err = proc.communicate(payload)

    with open(ERR_LOG, "w", encoding="utf-8") as f:
        f.write(err)

    responses = []
    for line in out.splitlines():
        if not line.strip():
            continue
        try:
            responses.append(json.loads(line))
        except json.JSONDecodeError:
            print(f"WARNING: non-JSON line in stdout: {line}")
    return responses


def tool_call(req_id, name, arguments=None):
    return json.dumps({
        "jsonrpc": "2.0",
        "id": req_id,
        "method": "tools/call",
        "params": {"name": name, "arguments": arguments or {}},
    })


def show_result(resp, title):
    if resp is None:
        return
    if "error" in resp:
        err = resp["error"]
        print(f"  {title:<10} : ERROR {err.get('code')}: {err.get('message')}")
        return
    result = resp.get("result", {})
    content = result.get("content")
    if content:
        text = content[0].get("text", "")
        if len(text) > 400:
            text = text[:400] + "..."
        mark = " (isError)" if result.get("isError") else ""
        print(f"  {title:<10} : {text}{mark}")
    else:
        print(f"  {title:<10} : {json.dumps(result, ensure_ascii=False)}")


def get_tool_json(resp, title):
    """Returns parsed tool-result JSON or None (prints the error on isError)."""
    if not resp or "result" not in resp:
        return None
    result = resp["result"]
    text = result.get("content", [{}])[0].get("text", "")
    if result.get("isError"):
        print(f"  {title:<10} : TOOL ERROR: {text}")
        return None
    try:
        return json.loads(text)
    except json.JSONDecodeError:
        print(f"  {title:<10} : unparseable tool result: {text}")
        return None


def by_id(responses):
    return {r.get("id"): r for r in responses}


class McpSession:
    """Interactive JSON-RPC session with one rsldbgmcp.exe instance.

    The debug session behind the server is one-shot, so dependent calls
    (e.g. expand a variable reported by rsl_locals) must reuse the same
    process. Requests are answered strictly in order, one line each.
    """

    def __init__(self, exe, env):
        self.proc = subprocess.Popen(
            [exe],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            env=env,
            text=True,
            encoding="utf-8",
            errors="replace",
        )

    def request(self, req):
        self.proc.stdin.write(req + "\n")
        self.proc.stdin.flush()
        line = self.proc.stdout.readline()
        if not line.strip():
            return None
        return json.loads(line)

    def call_tool(self, req_id, name, arguments=None):
        return self.request(tool_call(req_id, name, arguments))

    def close(self):
        try:
            self.proc.stdin.close()
            self.proc.wait(timeout=10)
        except Exception:
            self.proc.kill()
        err = self.proc.stderr.read()
        with open(ERR_LOG, "w", encoding="utf-8") as f:
            f.write(err)


def main():
    parser = argparse.ArgumentParser(description="rsldbgmcp.exe smoke/full test")
    parser.add_argument("--uri", help="rsldbg://host:port[?token=...]")
    parser.add_argument("--exe", default=DEFAULT_EXE)
    parser.add_argument("--qtenv", default=DEFAULT_QTENV)
    args = parser.parse_args()

    if not os.path.exists(args.exe):
        sys.exit(f"Not found: {args.exe} (build the rsldbgmcp project first)")

    env = load_qt_env(args.qtenv)

    # ================= Level 1: smoke test =================
    print("== Smoke test: initialize / tools/list / rsl_sessions ==")

    smoke = by_id(invoke_mcp([
        '{"jsonrpc":"2.0","id":1,"method":"initialize"}',
        '{"jsonrpc":"2.0","id":2,"method":"tools/list"}',
        tool_call(3, "rsl_sessions", {"timeout_ms": 2000}),
    ], args.exe, env))

    show_result(smoke.get(1), "initialize")

    tools_resp = smoke.get(2)
    if tools_resp and "result" in tools_resp:
        tools = tools_resp["result"].get("tools", [])
        names = ", ".join(t["name"] for t in tools)
        print(f"  tools/list : {len(tools)} tools: {names}")

    sessions = []
    sessions_resp = smoke.get(3)
    if sessions_resp and "result" in sessions_resp:
        raw = sessions_resp["result"].get("content", [{}])[0].get("text", "")
        print(f"  raw        : {raw}")
        try:
            sessions = [s for s in json.loads(raw) if s.get("port", 0) > 0]
        except json.JSONDecodeError:
            pass
    print(f"  rsl_sessions : usable sessions: {len(sessions)}")

    # ================= Level 2: full scenario =================
    uri = args.uri
    if not uri:
        if not sessions:
            print("\nNo live sessions - full scenario skipped.")
            print("Start the host app (client_mode = agent), run debugbreak, then retry.")
            return 0
        s = sessions[0]
        uri = f"rsldbg://127.0.0.1:{s['port']}"
        print(f"  auto-selected session: pid={s.get('pid')} exe={s.get('exe')} port={s.get('port')} -> {uri}")

    print(f"\n== Full scenario: {uri} ==")

    session = McpSession(args.exe, env)
    try:
        line1 = line2 = None
        expand_ok = None

        show_result(session.request('{"jsonrpc":"2.0","id":10,"method":"initialize"}'),
                    "initialize")

        show_result(session.call_tool(11, "rsl_connect", {"uri": uri}), "connect")

        snap = get_tool_json(session.call_tool(12, "rsl_wait_event", {"timeout_ms": 3000}),
                             "wait_event")
        if snap:
            line1 = snap.get("source", {}).get("line")
            print(f"  wait_event : event={snap.get('event', {}).get('event')} "
                  f"file={snap.get('source', {}).get('filename')} line={line1} "
                  f"frames={len(snap.get('frames', []))} vars={len(snap.get('vars', []))}")

        frames = get_tool_json(session.call_tool(13, "rsl_stack"), "stack")
        if frames:
            top = frames[0]
            print(f"  stack      : {len(frames)} frames; top: {top.get('func')} "
                  f"({top.get('file')}:{top.get('line')})")

        variables = get_tool_json(session.call_tool(14, "rsl_locals"), "locals")
        if variables:
            names = ", ".join(v.get("name", "?") for v in variables[:5])
            print(f"  locals     : {len(variables)} vars: {names}")

        show_result(session.call_tool(15, "rsl_continue", {"mode": "step_over"}),
                    "step_over")

        snap = get_tool_json(session.call_tool(16, "rsl_wait_event", {"timeout_ms": 3000}),
                             "wait_event")
        if snap:
            line2 = snap.get("source", {}).get("line")
            print(f"  wait_event : event={snap.get('event', {}).get('event')} "
                  f"file={snap.get('source', {}).get('filename')} line={line2}")

        # ---- Third stop: line 22, obj = TestClass() is now constructed ----
        # (hello.mac: line 20 creates the object; stepping over it gives us
        # a complex variable with children: classmember, arr, born, ...)
        show_result(session.call_tool(21, "rsl_continue", {"mode": "step_over"}),
                    "step_over")

        snap = get_tool_json(session.call_tool(22, "rsl_wait_event", {"timeout_ms": 3000}),
                             "wait_event")
        if snap:
            print(f"  wait_event : event={snap.get('event', {}).get('event')} "
                  f"file={snap.get('source', {}).get('filename')} "
                  f"line={snap.get('source', {}).get('line')}")

        # ---- Variable expansion: pick an object var from fresh locals ----
        variables = get_tool_json(session.call_tool(17, "rsl_locals"), "locals")
        target = None
        if variables:
            for v in variables:
                if v.get("name", "").upper() == "OBJ" and v.get("object"):
                    target = v
                    break
            if target is None:
                target = next((v for v in variables
                               if v.get("object") or v.get("fakeChildren")), None)

        if target is None:
            print("  expand     : no expandable variable found, skipped")
        else:
            print(f"  expand     : expanding {target.get('name')} "
                  f"(index={target.get('index')} object={target.get('object')})")
            expanded = get_tool_json(
                session.call_tool(18, "rsl_expand_variable",
                                  {"index": target.get("index"),
                                   "stack": target.get("st", "0"),
                                   "stackindex": 0}),
                "expand")
            if expanded is not None:
                children = [v for v in expanded
                            if v.get("depth", 0) > (target.get("depth", 0))]
                names = ", ".join(v.get("name", "?") for v in children[:8])
                print(f"  expand     : {len(variables)} -> {len(expanded)} vars, "
                      f"children of {target.get('name')}: "
                      f"{names if names else '(none)'}")
                expand_ok = len(children) > 0

                value = get_tool_json(
                    session.call_tool(19, "rsl_var_value",
                                      {"val": target.get("val", "0"),
                                       "info": target.get("info", "0")}),
                    "var_value")
                if value is not None:
                    text = json.dumps(value, ensure_ascii=False)
                    if len(text) > 300:
                        text = text[:300] + "..."
                    print(f"  var_value  : {text}")

        show_result(session.call_tool(20, "rsl_disconnect"), "disconnect")
    finally:
        session.close()

    print()
    ok = line1 is not None and line2 is not None and line1 != line2
    if ok:
        print(f"OK: step_over moved line {line1} -> {line2}")
    elif line1 is not None and line2 is not None:
        print(f"WARNING: line did not change ({line1} -> {line2}). Check the logs.")

    if expand_ok is True:
        print("OK: variable expansion returned children")
    elif expand_ok is False:
        print("WARNING: variable expansion returned no children. Check the logs.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
