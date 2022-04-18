#pragma once

#ifdef _DEBUG
#define ON_DEBUG
#else

#define ON_DEBUG

#endif // _DEBUG



#ifdef ON_DEBUG
#define UseImgui
#endif