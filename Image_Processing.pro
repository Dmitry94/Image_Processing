TEMPLATE = subdirs

SUBDIRS += \
    gui_library \
    icpl \
    lab_1_intro \
    lab_2_brightness

lab_1_intro.depends = icpl
lab_1_intro.depends = gui_library

lab_2_brightness.depends = icpl
lab_2_brightness.depends = gui_library
