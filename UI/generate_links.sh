#!/bin/bash

controls="
	Button 
	Canvas 
	CheckBox 
	ComboBox 
	Container 
	Label 
	ListBox 
	TextBox 
	ScrollBar 
	TabContainer 
	Terminal
	TrackBar
	Dialog
	RadioButton
	WidgetMover
	WidgetResizer
	ProgressBar
"

common_header_only_controls="
	Form
"

################ generate AllControls.hpp ##################################
echo "#ifndef ALL_CONTROLS_HPP" > AllControls.hpp
echo "#define ALL_CONTROLS_HPP" >> AllControls.hpp
echo "#ifdef USE_SDL" >> AllControls.hpp
for i in $controls; do
	echo -e "\t#include \"controls/SDL/$i.hpp\"" >> AllControls.hpp
done
echo "#elif USE_SFML" >> AllControls.hpp
for i in $controls; do
	echo -e "\t#include \"controls/SFML/$i.hpp\"" >> AllControls.hpp
done
echo "#endif" >> AllControls.hpp
for i in $common_header_only_controls; do
	echo "#include \"controls/$i.hpp\"" >> AllControls.hpp
done
echo "#endif" >> AllControls.hpp
###############################################################################

cd controls

# links are not working correctly because of relative paths in
# .hpp so it starts pointing to wrong relative path
if [ x$1 == x'--links' ]; then
	if [ x$2 != x ]; then
		for i in $controls; do 
			echo generating link $i.hpp "->" $2/$i.hpp
			ln -sf $2/$i.hpp .
		done
	fi
else
	for i in $controls; do 
		echo generating $i.hpp
		rm -f $i.hpp # make sure its not link there
		cat > $i.hpp << EOF
		#ifdef USE_SDL
		 #include "SDL/$i.hpp"
		#elif USE_SFML
		 #include "SFML/$i.hpp"
		#endif
EOF
		done
fi
