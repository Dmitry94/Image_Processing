TEMPLATE = subdirs

SUBDIRS += \
    gui_library \
    icpl \
    lab_1_hists \
    lab_2_bright

lab_1_intro.depends = icpl
lab_1_intro.depends = gui_library
