TEMPLATE = subdirs

SUBDIRS += \
    loki \
    rsldbg \
    dbgclient \
    rsldbgclnt \
    rsldbgmcp \
    rslexecutor \
    rslrun \
    rsledit

rsldbg.depends = loki
dbgclient.depends = rsldbg
rsldbgclnt.depends = dbgclient
rsldbgmcp.depends = dbgclient
# rsledit embeds the macro runner itself (through the ported RslExecutor
# static library), but needs rsldbg.dll and rsldbgclnt.exe deployed next to
# it (rsldbgclnt installs them into bin/).
rsledit.depends = rsldbgclnt rslexecutor
