# rsldbgmcp — MCP-сервер для отладки RSL

Консольный MCP-адаптер (JSON-RPC 2.0 поверх stdio) для отладки RSL-скриптов
внутри хост-приложений через `rsldbg.dll`. Запускается MCP-клиентом
(Claude Desktop, IDE) как дочерний процесс; сетевого порта не открывает.

```
AI-клиент ──stdio──> rsldbgmcp.exe ──TCP──> rsldbg.dll (отлаживаемый процесс)
```

## Конфигурация (пример для Claude Desktop)

```json
{
  "mcpServers": {
    "rsldbg": {
      "command": "D:/Work/RslDebugger/bin/rsldbgmcp.exe"
    }
  }
}
```

## Инструменты

| Инструмент | Назначение |
|---|---|
| `rsl_sessions` | Поиск живых отладочных сессий (UDP discovery, порт 51491) |
| `rsl_connect` | Подключение по `rsldbg://host:port?token=...` или host/port/token |
| `rsl_disconnect` | Отключение от сессии |
| `rsl_continue` | Управление выполнением: run, step_over, step_into, step_out, abort |
| `rsl_wait_event` | Ожидание останова/события, возвращает снапшот состояния |
| `rsl_locals` | Локальные переменные текущего кадра |
| `rsl_stack` | Стек вызовов |
| `rsl_breakpoints` | Точки останова |
| `rsl_source` | Текущий модуль: файл, строка, полный текст |
| `rsl_expand_variable` | Развернуть объектную переменную |
| `rsl_var_value` | Имя/тип/значение переменной по хендлам val/info |

## Проверка без MCP-клиента

```powershell
.\test-mcp.ps1           # дымовой тест + автопоиск сессии и полный сценарий
.\test-mcp.ps1 -Uri rsldbg://127.0.0.1:63873
```

Скрипт сам импортирует окружение Qt из `qtenv2.bat`, гоняет JSON-RPC
пакеты через stdin и печатает сводку ответов.

## Типовой сценарий агента

1. В хост-приложении выполняется `debugbreak` → `rsldbg.dll` поднимает
   сервер (в режиме `client_mode = agent` URI копируется в буфер обмена
   и пишется в лог).
2. `rsl_sessions` → список сессий (pid, port, exe).
3. `rsl_connect` с URI сессии.
4. `rsl_continue {mode: "step_over"}` → `rsl_wait_event` → `rsl_locals`,
   `rsl_stack`, `rsl_source` — итеративная отладка.
