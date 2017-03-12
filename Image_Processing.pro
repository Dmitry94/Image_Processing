TEMPLATE = subdirs

SUBDIRS += \
    gui_library \
    icpl \
    lab_1_intro

lab_1_Base.depends = icpl
lab_1_Base.depends = gui_library
