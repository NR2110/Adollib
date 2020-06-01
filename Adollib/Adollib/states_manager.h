#pragma once

namespace Adollib {

	class State_manager {

	public:
		//DepthStencilState
		static enum class DStypes { 
			DS_FALSE,
			DS_TRUE 
		};

		//RasterizerState
		static enum class RStypes { 
			RS_CULL_BACK,
			RS_WIRE, 
			RS_CULL_FRONT,
			RS_CULL_NONE, 
			RS_SIZE 
		};
		////RasterizerState
		//static enum class RStypes { 
		//	RS_CULL_NONE,
		//	RS_CULL_BACK,
		//	RS_CULL_FRONT,
		//	RS_WIRE_NONE,
		//	RS_WIRE_BACK,
		//	RS_WIRE_FRONT,
		//	RS_SIZE 
		//};

		//BlendState
		static enum class BStypes { 
			BS_NONE,  //��
			BS_ALPHA, //��Z
			BS_ADD,   //
			BS_SUBTRACT,// 
			BS_REPLACE, //
			BS_MULTIPLY, //
			BS_LIGHTEN, //
			BS_DARKEN, //
			BS_SCREEN, //
			BS_SIZE //
		};

	};
}

