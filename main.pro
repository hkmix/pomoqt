TEMPLATE = subdirs

SUBDIRS += pomolite \
           pomoqt

pomolite.subdir = src/pomolite
pomoqt.subdir = src/pomoqt

pomoqt.depends = pomolite

QMAKE_CXXFLAGS += -std=c++14 -Wall -Wextra -Wpedantic -Werror
