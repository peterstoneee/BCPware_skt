TEMPLATE = subdirs


TEMPLATE      = subdirs
CONFIG       += ordered


                           # the common framework, used by all the plugins,
SUBDIRS       = common \
                bcpware \                         # the GUI framework
                bcpwarePlugins/io_base\           # a few basic file formats (ply, obj, off), without this you cannot open anything
                #bcpwarePlugins/filter_meshing \
                bcpwarePlugins/io_x3d \
                bcpwarePlugins/filter_box_packing \
                bcpwarePlugins/generate_print_func\
                bcpwarePlugins/edit_test\
#
# Next some other useful, but still easy to be compiled, plugins
# Uncomment them if you succeed in compiling the above ones.
#               bcpwarePlugins/decorate_background \#o
#                bcpwarePlugins/decorate_base \#o
#                bcpwarePlugins/edit_align \
#                bcpwarePlugins/edit_manipulators \#o
#                bcpwarePlugins/edit_select \
#                bcpwarePlugins/filter_clean\
#                bcpwarePlugins/filter_colorize\
#                bcpwarePlugins/filter_create\
#                bcpwarePlugins/filter_layer\
#                bcpwarePlugins/filter_measure\
#                bcpwarePlugins/filter_sampling\
#                bcpwarePlugins/filter_select \
#                bcpwarePlugins/filter_quality \
#                bcpwarePlugins/filter_unsharp \
#                bcpwarePlugins/io_collada \

#                meshlabserver

#                bcpwarePlugins/render_radiance_scaling\#o

#               plugins_experimental/filter_output_optical_flow \#o

#                bcpwarePlugins/test_filtergpu\
#               bcpwarePlugins/samplefilter_2\#o
#                bcpwarePlugins/my_decorate \
#                bcpwarePlugins/io_print \
#               bcpwarePlugins/edit_paint





