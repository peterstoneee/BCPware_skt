#include "slice_program_setting.h"
#include "define_some_parameter.h"


slice_program_setting::slice_program_setting()
{
	top_Gap = 3;
	bottom_Gap = 3;
	left_Gap = 3;
	right_Gap = 3;
	between_Gap = 3;
	outlineImageName = "ImageFileName";
	capImageName = "CapImageFileName";	

	setOlMeshName("_temp_outlines");
	setCapMeshName("_temp_ol_mesh");

	genPicDir = "D:/temptemp/xyzimage";
	gd_unit = grid_unit::cm;
	print_pagesize = page_size::longPage_size;

	buildheight.label = "layer_height";
	buildheight.unit = "mm";
	buildheight.value = 0.5;
	print_or_not = false;
	useless_width = 1;
	plus_print_length = 13.4;
}
slice_program_setting::slice_program_setting(float t, float b, float r,float l, float bt)
{
	top_Gap = t;
	bottom_Gap = b;
	left_Gap = r;
	right_Gap = l;
	between_Gap = bt;
	outlineImageName = "ImageFileName";
	capImageName = "CapImageFileName";

	setOlMeshName("_temp_outlines");
	setCapMeshName("_temp_ol_mesh");

	genPicDir = "D:/temptemp/xyzimage";
	gd_unit = grid_unit::cm;
	print_pagesize = page_size::longPage_size;

	buildheight.label = "layer_height";
	buildheight.unit = "mm";
	buildheight.value = 0.5;
	print_or_not = false;
	useless_width = 1;
	plus_print_length = 13.4;
}


slice_program_setting::~slice_program_setting()
{
}

 int slice_program_setting::getTopGap() { return top_Gap; }
 float slice_program_setting::getBottom_Gap(){ return bottom_Gap; }
 float slice_program_setting::getLeft_Gap(){ return left_Gap; }
 float slice_program_setting::getRight_Gap(){ return right_Gap; }
 float slice_program_setting::getBetweenGap(){ return between_Gap; }

 void slice_program_setting::setTopGap(int _topGap){ top_Gap = _topGap; }
 void slice_program_setting::setBottomGap(float _BottomGap){ bottom_Gap = _BottomGap; }
 void slice_program_setting::setLeftGap(float _leftGap){ left_Gap = _leftGap; }
 void slice_program_setting::setRightGap(float _right_Gap){ right_Gap = _right_Gap; }
 void slice_program_setting::setBetweenGap(float _between_Gap){ between_Gap = _between_Gap; }


 void slice_program_setting::initValue(RichParameterSet &initval)
 {

	 initval.addParam(new RichFloat("groove_length", 22.2, "x", ""));
	 initval.addParam(new RichFloat("groove_height", 22.2, "y", ""));
	 initval.addParam(new RichFloat("groove_width", 20, "z", ""));
	 initval.addParam(new RichInt("Vertices_Number", 3000000, "Vertices Number", "bigger_use_Point_Draw"));


	 initval.addParam(new RichFloat("top_gap", 3, "top_gap", ""));
	 initval.addParam(new RichFloat("bottom_gap", 3, "bottom_gap", ""));
	 initval.addParam(new RichFloat("right_gap", 3, "right_gap", ""));
	 initval.addParam(new RichFloat("left_gap", 3, "left_gapr", ""));
	 initval.addParam(new RichFloat("between_gap", 2, "between_gap", ""));
	 initval.addParam(new RichString("dir", "test", "dir", ""));
	 initval.addParam(new RichEnum("grid_unit", 0, QStringList() << "centimeter" << "inch", ("grid_unit"), QString("")));//*
	 initval.addParam(new RichFloat("slice_Height", 0.5, "slice_height", ""));//*
	 initval.addParam(new RichBool("start_print", false, "start_print", ""));
	 initval.addParam(new RichFloat("useless_print", 1, "useless_print", ""));
	 initval.addParam(new RichEnum("print_size", 1, QStringList() << "printBed_size" << "long_page_size", ("print_size"), QString("")));
	 initval.addParam(new RichFloat("plus_print_length", DSP_plusPrintLength, "plus_print_length", ""));

 }

 void slice_program_setting::updateValue(RichParameterSet &set)
 {
	 setTopGap(set.getFloat("top_gap"));
	 setBottomGap(set.getFloat("bottom_gap"));
	 setLeftGap(set.getFloat("left_gap"));
	 setRightGap(set.getFloat("right_gap"));
	 setBetweenGap(set.getFloat("between_gap"));
	 Label_Float temp = getbuildheight();
	 temp.value = set.getFloat("slice_Height");
	 setbuildheight(temp);
	 	 

 }