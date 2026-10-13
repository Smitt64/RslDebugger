TEMPLATE = subdirs

SUBDIRS += \
    loki \
    rsldbg \
    rsldbgclnt \
    rslexecutor \
    rslrun \
    rsledit

rsldbg.depends = loki
rsldbgclnt.depends = rsldbg
# rsledit embeds the macro runner itself (through the ported RslExecutor
# static library), but needs rsldbg.dll and rsldbgclnt.exe deployed next to
# it (rsldbgclnt installs them into bin/).
rsledit.depends = rsldbgclnt rslexecutor
