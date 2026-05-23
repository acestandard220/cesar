#include "pch.h"

#include "FrameResources.h"
#include "RenderFrame.h"
#include "ResourceModifications.h"

using namespace Helpers;

void RenderFrame()
{
    ResetCommandAllocators();
    RenderFrame_000();

    // Some events like DispatchRays() can create new resources per frame.
    // Thus, these resources need to be released after GPU is done with them.
    g_perFrameBuffers.clear();
}