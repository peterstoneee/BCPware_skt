/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2005                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/


/****************************************************************************
* My Extension to Meshlab
*
*  Copyright 2018 by
*
* This file is part of Meshlab.
*
* This program is free software: you can redistribute
* it and/or modify it under the terms of the GNU General Public
* License as published by the Free Software Foundation, either
* version 3 of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will
* be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* @license GPL-3.0+ <http://spdx.org/licenses/GPL-3.0+>
****************************************************************************/

#include "../common/filterparameter.h"
#include "glarea_setting.h"

RichParameterSet GLAreaSetting::glAreaSet;
void GLAreaSetting::initGlobalParameterSet( RichParameterSet * defaultGlobalParamSet)
{
 //   defaultGlobalParamSet->addParam(new RichColor(backgroundBotColorParam(),QColor(0,170,0),"MeshLab Bottom BackGround Color","MeshLab GLarea's BackGround Color(bottom corner)"));
 //   //defaultGlobalParamSet->addParam(new RichColor(backgroundTopColorParam(),QColor(  0, 0,   0),"MeshLab Top BackGround Color","MeshLab GLarea's BackGround Color(top corner)"));
	//defaultGlobalParamSet->addParam(new RichColor(backgroundTopColorParam(),QColor( 0 , 170,   0),"MeshLab Top BackGround Color","MeshLab GLarea's BackGround Color(top corner)"));
 //   defaultGlobalParamSet->addParam(new RichColor(logAreaColorParam(),   QColor(255,16,16),"MeshLab GLarea's Log Area Color","MeshLab GLarea's BackGround Color(bottom corner)"));
 //   defaultGlobalParamSet->addParam(new RichColor(textColorParam(),vcg::Color4b::White,"Text Color","Color of the text used in all the Graphics Window (it should be well different from the background color...)"));
	//

 //   defaultGlobalParamSet->addParam(new RichColor(baseLightAmbientColorParam()	,QColor( 32, 32, 32),"MeshLab Base Light Ambient Color","MeshLab GLarea's BackGround Color(bottom corner)"));
 //   //defaultGlobalParamSet->addParam(new RichColor(baseLightDiffuseColorParam()	,QColor(204,204,204),"MeshLab Base Light Diffuse Color","MeshLab GLarea's BackGround Color(top corner)"));
	//defaultGlobalParamSet->addParam(new RichColor(baseLightDiffuseColorParam(), QColor(255, 255, 255), "MeshLab Base Light Diffuse Color", "MeshLab GLarea's BackGround Color(top corner)"));
 //   defaultGlobalParamSet->addParam(new RichColor(baseLightSpecularColorParam() ,QColor(255,255,255),"MeshLab Base Light Specular Color","MeshLab GLarea's BackGround Color(bottom corner)"));

 //   //defaultGlobalParamSet->addParam(new RichColor(fancyBLightDiffuseColorParam()	,QColor(255,204,204),"MeshLab Base Light Diffuse Color","MeshLab GLarea's BackGround Color(top corner)"));
 //   //defaultGlobalParamSet->addParam(new RichColor(fancyFLightDiffuseColorParam()	,QColor(204,204,255),"MeshLab Base Light Diffuse Color","MeshLab GLarea's BackGround Color(top corner)"));
	//defaultGlobalParamSet->addParam(new RichColor(fancyBLightDiffuseColorParam(), QColor(255, 255, 255), "MeshLab Base Light Diffuse Color", "MeshLab GLarea's BackGround Color(top corner)"));
	//defaultGlobalParamSet->addParam(new RichColor(fancyFLightDiffuseColorParam(), QColor(255, 255, 255), "MeshLab Base Light Diffuse Color", "MeshLab GLarea's BackGround Color(top corner)"));

 //   QStringList textureMinFilterModes =  (QStringList() << "Nearest" << "MipMap");
 //   QStringList textureMagFilterModes =  (QStringList() << "Nearest" << "Linear");
 //   defaultGlobalParamSet->addParam(new RichEnum(textureMinFilterParam()	, 1,textureMinFilterModes,"MeshLab Texture Minification Filtering","MeshLab GLarea's BackGround Color(top corner)"));
 //   defaultGlobalParamSet->addParam(new RichEnum(textureMagFilterParam()	, 1,textureMagFilterModes,"MeshLab Texture Magnification Filtering","MeshLab GLarea's BackGround Color(top corner)"));

 //   defaultGlobalParamSet->addParam(new RichBool(pointDistanceAttenuationParam()	, true,"Perspective Varying Point Size","If true the size of the points is drawn with a size proprtional to the distance from the observer."));
 //   defaultGlobalParamSet->addParam(new RichBool(pointSmoothParam()	, false,"Antialiased Point","If true the points are drawn with small circles instead of fast squared dots."));
 //   defaultGlobalParamSet->addParam(new RichFloat(pointSizeParam()	, 2.0, "Point Size","The base size of points when drawn"));
 //   defaultGlobalParamSet->addParam(new RichInt(maxTextureMemoryParam()	, 256, "Max Texture Memory (in MB)","The maximum quantity of texture memory allowed to load mesh textures"));
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	glAreaSet.addParam(new RichColor(backgroundBotColorParam(), QColor(0, 170, 0), "MeshLab Bottom BackGround Color", "MeshLab GLarea's BackGround Color(bottom corner)"));
	
	glAreaSet.addParam(new RichColor(backgroundTopColorParam(), QColor(0, 170, 0), "MeshLab Top BackGround Color", "MeshLab GLarea's BackGround Color(top corner)"));
	glAreaSet.addParam(new RichColor(logAreaColorParam(), QColor(255, 16, 16), "MeshLab GLarea's Log Area Color", "MeshLab GLarea's BackGround Color(bottom corner)"));
	glAreaSet.addParam(new RichColor(textColorParam(), vcg::Color4b::White, "Text Color", "Color of the text used in all the Graphics Window (it should be well different from the background color...)"));


	glAreaSet.addParam(new RichColor(baseLightAmbientColorParam(), QColor(32, 32, 32), "MeshLab Base Light Ambient Color", "MeshLab GLarea's BackGround Color(bottom corner)"));
	//defaultGlobalParamSet->addParam(new RichColor(baseLightDiffuseColorParam()	,QColor(204,204,204),"MeshLab Base Light Diffuse Color","MeshLab GLarea's BackGround Color(top corner)"));
	glAreaSet.addParam(new RichColor(baseLightDiffuseColorParam(), QColor(255, 255, 255), "MeshLab Base Light Diffuse Color", "MeshLab GLarea's BackGround Color(top corner)"));
	glAreaSet.addParam(new RichColor(baseLightSpecularColorParam(), QColor(255, 255, 255), "MeshLab Base Light Specular Color", "MeshLab GLarea's BackGround Color(bottom corner)"));

	//defaultGlobalParamSet->addParam(new RichColor(fancyBLightDiffuseColorParam()	,QColor(255,204,204),"MeshLab Base Light Diffuse Color","MeshLab GLarea's BackGround Color(top corner)"));
	//defaultGlobalParamSet->addParam(new RichColor(fancyFLightDiffuseColorParam()	,QColor(204,204,255),"MeshLab Base Light Diffuse Color","MeshLab GLarea's BackGround Color(top corner)"));
	glAreaSet.addParam(new RichColor(fancyBLightDiffuseColorParam(), QColor(255, 255, 255), "MeshLab Base Light Diffuse Color", "MeshLab GLarea's BackGround Color(top corner)"));
	glAreaSet.addParam(new RichColor(fancyFLightDiffuseColorParam(), QColor(255, 255, 255), "MeshLab Base Light Diffuse Color", "MeshLab GLarea's BackGround Color(top corner)"));

	QStringList textureMinFilterModes = (QStringList() << "Nearest" << "MipMap");
	QStringList textureMagFilterModes = (QStringList() << "Nearest" << "Linear");
	glAreaSet.addParam(new RichEnum(textureMinFilterParam(), 1, textureMinFilterModes, "MeshLab Texture Minification Filtering", "MeshLab GLarea's BackGround Color(top corner)"));
	glAreaSet.addParam(new RichEnum(textureMagFilterParam(), 1, textureMagFilterModes, "MeshLab Texture Magnification Filtering", "MeshLab GLarea's BackGround Color(top corner)"));

	glAreaSet.addParam(new RichBool(pointDistanceAttenuationParam(), true, "Perspective Varying Point Size", "If true the size of the points is drawn with a size proprtional to the distance from the observer."));
	glAreaSet.addParam(new RichBool(pointSmoothParam(), false, "Antialiased Point", "If true the points are drawn with small circles instead of fast squared dots."));
	glAreaSet.addParam(new RichFloat(pointSizeParam(), 2.0, "Point Size", "The base size of points when drawn"));
	glAreaSet.addParam(new RichInt(maxTextureMemoryParam(), 256, "Max Texture Memory (in MB)", "The maximum quantity of texture memory allowed to load mesh textures"));
}


void GLAreaSetting::updateGlobalParameterSet( RichParameterSet& rps )
{
    /*logAreaColor = rps.getColor4b(logAreaColorParam());
    backgroundBotColor =  rps.getColor4b(backgroundBotColorParam());
    backgroundTopColor =  rps.getColor4b(backgroundTopColorParam());

    baseLightAmbientColor =  rps.getColor4b(baseLightAmbientColorParam()	);
    baseLightDiffuseColor =  rps.getColor4b(baseLightDiffuseColorParam()	);
    baseLightSpecularColor =  rps.getColor4b(baseLightSpecularColorParam() );

    fancyBLightDiffuseColor =  rps.getColor4b(fancyBLightDiffuseColorParam());
    fancyFLightDiffuseColor =  rps.getColor4b(fancyFLightDiffuseColorParam());

    textureMinFilter = rps.getEnum(this->textureMinFilterParam());
    textureMagFilter = rps.getEnum(this->textureMagFilterParam());

    pointDistanceAttenuation = rps.getBool(this->pointDistanceAttenuationParam());
    pointSmooth = rps.getBool(this->pointSmoothParam());
    pointSize = rps.getFloat(this->pointSizeParam());
    maxTextureMemory = rps.getInt(this->maxTextureMemoryParam());
    currentGlobalParamSet=&rps;*/

	logAreaColor = vcg::Color4b(255, 16, 16, 0);
	backgroundBotColor = vcg::Color4b(0, 170, 0, 0);
	backgroundTopColor = vcg::Color4b(0, 170, 0, 0);

	baseLightAmbientColor = vcg::Color4b(32, 32, 32, 0);
	baseLightDiffuseColor = vcg::Color4b(255, 255, 255, 0);
	baseLightSpecularColor = vcg::Color4b(255, 255, 255, 0);

	fancyBLightDiffuseColor = vcg::Color4b(255, 255, 255, 0);
	fancyFLightDiffuseColor = vcg::Color4b(255, 255, 255, 0);

	textureMinFilter = 1;
	textureMagFilter = 1;

	pointDistanceAttenuation = true;
	pointSmooth = false;
	pointSize = 2.0;
	maxTextureMemory = 256;
	
}
