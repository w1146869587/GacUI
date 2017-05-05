﻿/***********************************************************************
!!!!!! DO NOT MODIFY !!!!!!

Source: Host.sln

This file is generated by Workflow compiler
https://github.com/vczh-libraries
***********************************************************************/

#ifndef VCZH_WORKFLOW_COMPILER_GENERATED_DEMOINCLUDES
#define VCZH_WORKFLOW_COMPILER_GENERATED_DEMOINCLUDES

#include "Demo.h"
#include "MainWindow.h"

/***********************************************************************
Reflection
***********************************************************************/

namespace vl
{
	namespace reflection
	{
		namespace description
		{
#ifndef VCZH_DEBUG_NO_REFLECTION
			DECL_TYPE_INFO(::demo::IViewModel)
			DECL_TYPE_INFO(::demo::MainWindow)
			DECL_TYPE_INFO(::demo::MainWindowConstructor)
			DECL_TYPE_INFO(::demo::MyTextItem)
			DECL_TYPE_INFO(::demo::TextListTabPage)
			DECL_TYPE_INFO(::demo::TextListTabPageConstructor)

			BEGIN_INTERFACE_PROXY_NOPARENT_SHAREDPTR(::demo::IViewModel)
				::vl::Ptr<::vl::reflection::description::IValueObservableList> GetTextItems() override
				{
					INVOKEGET_INTERFACE_PROXY_NOPARAMS(GetTextItems);
				}
			END_INTERFACE_PROXY(::demo::IViewModel)
#endif

			extern bool LoadDemoTypes();
		}
	}
}

#endif
