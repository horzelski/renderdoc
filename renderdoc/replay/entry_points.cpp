/******************************************************************************
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2017 Baldur Karlsson
 * Copyright (c) 2014 Crytek
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/

#include <sstream>
#include "api/replay/renderdoc_replay.h"
#include "api/replay/version.h"
#include "common/common.h"
#include "core/core.h"
#include "maths/camera.h"
#include "maths/formatpacking.h"
#include "replay/type_helpers.h"
#include "serialise/string_utils.h"

// these entry points are for the replay/analysis side - not for the application.

extern "C" RENDERDOC_API uint32_t RENDERDOC_CC Topology_NumVerticesPerPrimitive(Topology topology)
{
  // strips/loops/fans have the same number of indices for a single primitive
  // as their list friends
  switch(topology)
  {
    default:
    case Topology::Unknown: break;
    case Topology::PointList: return 1;
    case Topology::LineList:
    case Topology::LineStrip:
    case Topology::LineLoop: return 2;
    case Topology::TriangleList:
    case Topology::TriangleStrip:
    case Topology::TriangleFan: return 3;
    case Topology::LineList_Adj:
    case Topology::LineStrip_Adj: return 4;
    case Topology::TriangleList_Adj:
    case Topology::TriangleStrip_Adj: return 6;
    case Topology::PatchList_1CPs:
    case Topology::PatchList_2CPs:
    case Topology::PatchList_3CPs:
    case Topology::PatchList_4CPs:
    case Topology::PatchList_5CPs:
    case Topology::PatchList_6CPs:
    case Topology::PatchList_7CPs:
    case Topology::PatchList_8CPs:
    case Topology::PatchList_9CPs:
    case Topology::PatchList_10CPs:
    case Topology::PatchList_11CPs:
    case Topology::PatchList_12CPs:
    case Topology::PatchList_13CPs:
    case Topology::PatchList_14CPs:
    case Topology::PatchList_15CPs:
    case Topology::PatchList_16CPs:
    case Topology::PatchList_17CPs:
    case Topology::PatchList_18CPs:
    case Topology::PatchList_19CPs:
    case Topology::PatchList_20CPs:
    case Topology::PatchList_21CPs:
    case Topology::PatchList_22CPs:
    case Topology::PatchList_23CPs:
    case Topology::PatchList_24CPs:
    case Topology::PatchList_25CPs:
    case Topology::PatchList_26CPs:
    case Topology::PatchList_27CPs:
    case Topology::PatchList_28CPs:
    case Topology::PatchList_29CPs:
    case Topology::PatchList_30CPs:
    case Topology::PatchList_31CPs:
    case Topology::PatchList_32CPs: return PatchList_Count(topology);
  }

  return 0;
}

extern "C" RENDERDOC_API uint32_t RENDERDOC_CC Topology_VertexOffset(Topology topology,
                                                                     uint32_t primitive)
{
  // strips/loops/fans have the same number of indices for a single primitive
  // as their list friends
  switch(topology)
  {
    default:
    case Topology::Unknown:
    case Topology::PointList:
    case Topology::LineList:
    case Topology::TriangleList:
    case Topology::LineList_Adj:
    case Topology::TriangleList_Adj:
    case Topology::PatchList_1CPs:
    case Topology::PatchList_2CPs:
    case Topology::PatchList_3CPs:
    case Topology::PatchList_4CPs:
    case Topology::PatchList_5CPs:
    case Topology::PatchList_6CPs:
    case Topology::PatchList_7CPs:
    case Topology::PatchList_8CPs:
    case Topology::PatchList_9CPs:
    case Topology::PatchList_10CPs:
    case Topology::PatchList_11CPs:
    case Topology::PatchList_12CPs:
    case Topology::PatchList_13CPs:
    case Topology::PatchList_14CPs:
    case Topology::PatchList_15CPs:
    case Topology::PatchList_16CPs:
    case Topology::PatchList_17CPs:
    case Topology::PatchList_18CPs:
    case Topology::PatchList_19CPs:
    case Topology::PatchList_20CPs:
    case Topology::PatchList_21CPs:
    case Topology::PatchList_22CPs:
    case Topology::PatchList_23CPs:
    case Topology::PatchList_24CPs:
    case Topology::PatchList_25CPs:
    case Topology::PatchList_26CPs:
    case Topology::PatchList_27CPs:
    case Topology::PatchList_28CPs:
    case Topology::PatchList_29CPs:
    case Topology::PatchList_30CPs:
    case Topology::PatchList_31CPs:
    case Topology::PatchList_32CPs:
      // for all lists, it's just primitive * Topology_NumVerticesPerPrimitive(topology)
      break;
    case Topology::LineStrip:
    case Topology::LineLoop:
    case Topology::TriangleStrip:
    case Topology::LineStrip_Adj:
      // for strips, each new vertex creates a new primitive
      return primitive;
    case Topology::TriangleStrip_Adj:
      // triangle strip with adjacency is a special case as every other
      // vert is purely for adjacency so it's doubled
      return primitive * 2;
    case Topology::TriangleFan: RDCERR("Cannot get VertexOffset for triangle fan!"); break;
  }

  return primitive * Topology_NumVerticesPerPrimitive(topology);
}

extern "C" RENDERDOC_API float RENDERDOC_CC Maths_HalfToFloat(uint16_t half)
{
  return ConvertFromHalf(half);
}

extern "C" RENDERDOC_API uint16_t RENDERDOC_CC Maths_FloatToHalf(float f)
{
  return ConvertToHalf(f);
}

extern "C" RENDERDOC_API Camera *RENDERDOC_CC Camera_InitArcball()
{
  return new Camera(Camera::eType_Arcball);
}

extern "C" RENDERDOC_API Camera *RENDERDOC_CC Camera_InitFPSLook()
{
  return new Camera(Camera::eType_FPSLook);
}

extern "C" RENDERDOC_API void RENDERDOC_CC Camera_Shutdown(Camera *c)
{
  delete c;
}

extern "C" RENDERDOC_API void RENDERDOC_CC Camera_SetPosition(Camera *c, float x, float y, float z)
{
  c->SetPosition(Vec3f(x, y, z));
}

extern "C" RENDERDOC_API void RENDERDOC_CC Camera_SetFPSRotation(Camera *c, float x, float y, float z)
{
  c->SetFPSRotation(Vec3f(x, y, z));
}

extern "C" RENDERDOC_API void RENDERDOC_CC Camera_SetArcballDistance(Camera *c, float dist)
{
  c->SetArcballDistance(dist);
}

extern "C" RENDERDOC_API void RENDERDOC_CC Camera_ResetArcball(Camera *c)
{
  c->ResetArcball();
}

extern "C" RENDERDOC_API void RENDERDOC_CC Camera_RotateArcball(Camera *c, float ax, float ay,
                                                                float bx, float by)
{
  c->RotateArcball(Vec2f(ax, ay), Vec2f(bx, by));
}

extern "C" RENDERDOC_API void RENDERDOC_CC Camera_GetBasis(Camera *c, FloatVector *pos,
                                                           FloatVector *fwd, FloatVector *right,
                                                           FloatVector *up)
{
  Vec3f p = c->GetPosition();
  Vec3f f = c->GetForward();
  Vec3f r = c->GetRight();
  Vec3f u = c->GetUp();

  pos->x = p.x;
  pos->y = p.y;
  pos->z = p.z;

  fwd->x = f.x;
  fwd->y = f.y;
  fwd->z = f.z;

  right->x = r.x;
  right->y = r.y;
  right->z = r.z;

  up->x = u.x;
  up->y = u.y;
  up->z = u.z;
}

extern "C" RENDERDOC_API const char *RENDERDOC_CC RENDERDOC_GetVersionString()
{
  return MAJOR_MINOR_VERSION_STRING;
}

extern "C" RENDERDOC_API const char *RENDERDOC_CC RENDERDOC_GetConfigSetting(const char *name)
{
  return RenderDoc::Inst().GetConfigSetting(name).c_str();
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_SetConfigSetting(const char *name,
                                                                      const char *value)
{
  RenderDoc::Inst().SetConfigSetting(name, value);
}

extern "C" RENDERDOC_API void *RENDERDOC_CC RENDERDOC_MakeEnvironmentModificationList(int numElems)
{
  rdctype::array<EnvironmentModification> *ret = new rdctype::array<EnvironmentModification>();
  create_array_uninit(*ret, (size_t)numElems);
  return ret;
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_SetEnvironmentModification(
    void *mem, int idx, const char *variable, const char *value, EnvMod type, EnvSep separator)
{
  rdctype::array<EnvironmentModification> *mods = (rdctype::array<EnvironmentModification> *)mem;

  mods->elems[idx] = EnvironmentModification(type, separator, variable, value);
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_FreeEnvironmentModificationList(void *mem)
{
  rdctype::array<EnvironmentModification> *mods = (rdctype::array<EnvironmentModification> *)mem;
  delete mods;
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_SetDebugLogFile(const char *log)
{
  if(log)
    RDCLOGFILE(log);
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_LogText(const char *text)
{
  rdclog_int(LogType::Comment, "EXT", "external", 0, "%s", text);
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_LogMessage(LogType type, const char *project,
                                                                const char *file, unsigned int line,
                                                                const char *text)
{
  rdclog_int(type, project ? project : "UNK?", file ? file : "unknown", line, "%s", text);

#if ENABLED(DEBUGBREAK_ON_ERROR_LOG)
  if(type == LogType::Error)
    RDCBREAK();
#endif

  if(type == LogType::Fatal)
    RDCDUMP();
}

extern "C" RENDERDOC_API const char *RENDERDOC_CC RENDERDOC_GetLogFile()
{
  return RDCGETLOGFILE();
}

extern "C" RENDERDOC_API void RENDERDOC_CC
RENDERDOC_InitGlobalEnv(GlobalEnvironment env, const rdctype::array<rdctype::str> &args)
{
  std::vector<std::string> argsVec;
  argsVec.reserve(args.size());
  for(const rdctype::str &a : args)
    argsVec.push_back(a.c_str());

  RenderDoc::Inst().ProcessGlobalEnvironment(env, argsVec);
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_TriggerExceptionHandler(void *exceptionPtrs,
                                                                             bool32 crashed)
{
  if(RenderDoc::Inst().GetCrashHandler() == NULL)
    return;

  if(exceptionPtrs)
  {
    RenderDoc::Inst().GetCrashHandler()->WriteMinidump(exceptionPtrs);
  }
  else
  {
    if(!crashed)
    {
      RDCLOG("Writing crash log");
    }

    RenderDoc::Inst().GetCrashHandler()->WriteMinidump();

    if(!crashed)
    {
      RenderDoc::Inst().RecreateCrashHandler();
    }
  }
}

extern "C" RENDERDOC_API ReplaySupport RENDERDOC_CC RENDERDOC_SupportLocalReplay(
    const char *logfile, rdctype::str *driver, rdctype::str *recordMachineIdent)
{
  ICaptureFile *file = RENDERDOC_OpenCaptureFile(logfile);

  if(driver)
    *driver = file->DriverName();

  if(recordMachineIdent)
    *recordMachineIdent = file->RecordedMachineIdent();

  ReplaySupport support = file->LocalReplaySupport();

  file->Shutdown();

  return support;
}

extern "C" RENDERDOC_API ReplayStatus RENDERDOC_CC
RENDERDOC_CreateReplayRenderer(const char *logfile, float *progress, IReplayController **rend)
{
  ICaptureFile *file = RENDERDOC_OpenCaptureFile(logfile);

  ReplayStatus ret = file->OpenStatus();

  if(ret != ReplayStatus::Succeeded)
  {
    file->Shutdown();
    return ret;
  }

  std::tie(ret, *rend) = file->OpenCapture(progress);

  file->Shutdown();

  return ret;
}

extern "C" RENDERDOC_API uint32_t RENDERDOC_CC
RENDERDOC_ExecuteAndInject(const char *app, const char *workingDir, const char *cmdLine,
                           const rdctype::array<EnvironmentModification> &env, const char *logfile,
                           const CaptureOptions &opts, bool32 waitForExit)
{
  return Process::LaunchAndInjectIntoProcess(app, workingDir, cmdLine, env, logfile, opts,
                                             waitForExit != 0);
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_GetDefaultCaptureOptions(CaptureOptions *opts)
{
  *opts = CaptureOptions();
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_StartGlobalHook(const char *pathmatch,
                                                                     const char *logfile,
                                                                     const CaptureOptions &opts)
{
  Process::StartGlobalHook(pathmatch, logfile, opts);
}

extern "C" RENDERDOC_API uint32_t RENDERDOC_CC
RENDERDOC_InjectIntoProcess(uint32_t pid, const rdctype::array<EnvironmentModification> &env,
                            const char *logfile, const CaptureOptions &opts, bool32 waitForExit)
{
  return Process::InjectIntoProcess(pid, env, logfile, opts, waitForExit != 0);
}

extern "C" RENDERDOC_API bool32 RENDERDOC_CC RENDERDOC_GetThumbnail(const char *filename,
                                                                    FileType type, uint32_t maxsize,
                                                                    rdctype::array<byte> *buf)
{
  ICaptureFile *file = RENDERDOC_OpenCaptureFile(filename);

  if(file->OpenStatus() != ReplayStatus::Succeeded)
  {
    file->Shutdown();
    return false;
  }

  *buf = file->GetThumbnail(type, maxsize);
  file->Shutdown();
  return true;
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_FreeArrayMem(const void *mem)
{
  rdctype::array<char>::deallocate(mem);
}

extern "C" RENDERDOC_API void *RENDERDOC_CC RENDERDOC_AllocArrayMem(uint64_t sz)
{
  return rdctype::array<char>::allocate((size_t)sz);
}

extern "C" RENDERDOC_API uint32_t RENDERDOC_CC RENDERDOC_EnumerateRemoteTargets(const char *host,
                                                                                uint32_t nextIdent)
{
  string s = "localhost";
  if(host != NULL && host[0] != '\0')
    s = host;

  // initial case is we're called with 0, start with the first port.
  // otherwise we're called with the last successful ident, so increment
  // before continuing to enumerate.
  if(nextIdent == 0)
    nextIdent = RenderDoc_FirstTargetControlPort;
  else
    nextIdent++;

  uint32_t lastIdent = RenderDoc_LastTargetControlPort;
  if(host != NULL && Android::IsHostADB(host))
  {
    if(nextIdent == RenderDoc_FirstTargetControlPort)
      nextIdent += RenderDoc_AndroidPortOffset;
    lastIdent += RenderDoc_AndroidPortOffset;

    s = "127.0.0.1";

    // could parse out an (optional) device name from host+4 here.
  }

  for(; nextIdent <= lastIdent; nextIdent++)
  {
    Network::Socket *sock = Network::CreateClientSocket(s.c_str(), (uint16_t)nextIdent, 250);

    if(sock)
    {
      SAFE_DELETE(sock);
      return nextIdent;
    }
  }

  // tried all idents remaining and found nothing
  return 0;
}

extern "C" RENDERDOC_API uint32_t RENDERDOC_CC RENDERDOC_GetDefaultRemoteServerPort()
{
  return RenderDoc_RemoteServerPort;
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_BecomeRemoteServer(const char *listenhost,
                                                                        uint32_t port,
                                                                        volatile bool32 *killReplay)
{
  bool32 dummy = false;

  if(killReplay == NULL)
    killReplay = &dummy;

  if(listenhost == NULL || listenhost[0] == 0)
    listenhost = "0.0.0.0";

  if(port == 0)
    port = RENDERDOC_GetDefaultRemoteServerPort();

  RenderDoc::Inst().BecomeRemoteServer(listenhost, (uint16_t)port, *killReplay);
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_StartSelfHostCapture(const char *dllname)
{
  void *module = Process::LoadModule(dllname);

  if(module == NULL)
    return;

  pRENDERDOC_GetAPI get =
      (pRENDERDOC_GetAPI)Process::GetFunctionAddress(module, "RENDERDOC_GetAPI");

  if(get == NULL)
    return;

  RENDERDOC_API_1_0_0 *rdoc = NULL;

  get(eRENDERDOC_API_Version_1_0_0, (void **)&rdoc);

  if(rdoc == NULL)
    return;

  rdoc->StartFrameCapture(NULL, NULL);
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_EndSelfHostCapture(const char *dllname)
{
  void *module = Process::LoadModule(dllname);

  if(module == NULL)
    return;

  pRENDERDOC_GetAPI get =
      (pRENDERDOC_GetAPI)Process::GetFunctionAddress(module, "RENDERDOC_GetAPI");

  if(get == NULL)
    return;

  RENDERDOC_API_1_0_0 *rdoc = NULL;

  get(eRENDERDOC_API_Version_1_0_0, (void **)&rdoc);

  if(rdoc == NULL)
    return;

  rdoc->EndFrameCapture(NULL, NULL);
}

namespace Android
{
bool IsHostADB(const char *hostname)
{
  return !strncmp(hostname, "adb:", 4);
}
string adbExecCommand(const string &args)
{
  string adbExePath = RenderDoc::Inst().GetConfigSetting("adbExePath");
  if(adbExePath.empty())
  {
    static bool warnPath = true;
    if(warnPath)
    {
      RDCWARN("adbExePath not set, attempting to call 'adb' in working env");
      warnPath = false;
    }
    adbExePath.append("adb");
  }
  Process::ProcessResult result;
  Process::LaunchProcess(adbExePath.c_str(), "", args.c_str(), &result);
  RDCLOG("COMMAND: adb %s", args.c_str());
  if(result.strStdout.length())
    // This could be an error (i.e. no package), or just regular output from adb devices.
    RDCLOG("STDOUT:\n%s", result.strStdout.c_str());
  return result.strStdout;
}
void adbForwardPorts()
{
  adbExecCommand(StringFormat::Fmt("forward tcp:%i tcp:%i",
                                   RenderDoc_RemoteServerPort + RenderDoc_AndroidPortOffset,
                                   RenderDoc_RemoteServerPort));
  adbExecCommand(StringFormat::Fmt("forward tcp:%i tcp:%i",
                                   RenderDoc_FirstTargetControlPort + RenderDoc_AndroidPortOffset,
                                   RenderDoc_FirstTargetControlPort));
}
uint32_t StartAndroidPackageForCapture(const char *host, const char *package)
{
  string packageName = basename(string(package));    // Remove leading '/' if any

  adbExecCommand("shell am force-stop " + packageName);
  adbForwardPorts();
  adbExecCommand("shell setprop debug.vulkan.layers VK_LAYER_RENDERDOC_Capture");
  adbExecCommand("shell pm grant " + packageName +
                 " android.permission.WRITE_EXTERNAL_STORAGE");    // Creating the capture file
  adbExecCommand("shell pm grant " + packageName +
                 " android.permission.READ_EXTERNAL_STORAGE");    // Reading the capture thumbnail
  adbExecCommand("shell monkey -p " + packageName + " -c android.intent.category.LAUNCHER 1");

  uint32_t ret = RenderDoc_FirstTargetControlPort + RenderDoc_AndroidPortOffset;
  uint32_t elapsed = 0,
           timeout = 1000 *
                     RDCMAX(5, atoi(RenderDoc::Inst().GetConfigSetting("MaxConnectTimeout").c_str()));
  while(elapsed < timeout)
  {
    // Check if the target app has started yet and we can connect to it.
    ITargetControl *control = RENDERDOC_CreateTargetControl(host, ret, "testConnection", false);
    if(control)
    {
      control->Shutdown();
      break;
    }

    Threading::Sleep(1000);
    elapsed += 1000;
  }

  // Let the app pickup the setprop before we turn it back off for replaying.
  adbExecCommand("shell setprop debug.vulkan.layers :");

  return ret;
}
}

using namespace Android;
extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_EnumerateAndroidDevices(rdctype::str *deviceList)
{
  string adbStdout = adbExecCommand("devices");

  using namespace std;
  istringstream stdoutStream(adbStdout);
  string ret;
  string line;
  while(getline(stdoutStream, line))
  {
    vector<string> tokens;
    split(line, tokens, '\t');
    if(tokens.size() == 2 && trim(tokens[1]) == "device")
    {
      if(ret.length())
        ret += ",";
      ret += tokens[0];
    }
  }

  if(ret.size())
    adbForwardPorts();    // Forward the ports so we can see if a remoteserver/captured app is
                          // already running.

  *deviceList = ret;
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_StartAndroidRemoteServer()
{
  adbExecCommand("shell am force-stop org.renderdoc.renderdoccmd");
  adbForwardPorts();
  adbExecCommand("shell setprop debug.vulkan.layers :");
  adbExecCommand(
      "shell am start -n org.renderdoc.renderdoccmd/.Loader -e renderdoccmd remoteserver");
}

extern "C" RENDERDOC_API bool RENDERDOC_CC RENDERDOC_NeedVulkanLayerRegistration(
    VulkanLayerFlags *flagsPtr, rdctype::array<rdctype::str> *myJSONsPtr,
    rdctype::array<rdctype::str> *otherJSONsPtr)
{
  VulkanLayerFlags flags = VulkanLayerFlags::NoFlags;
  std::vector<std::string> myJSONs;
  std::vector<std::string> otherJSONs;

  bool ret = RenderDoc::Inst().NeedVulkanLayerRegistration(flags, myJSONs, otherJSONs);

  if(flagsPtr)
    *flagsPtr = flags;

  if(myJSONsPtr)
  {
    create_array(*myJSONsPtr, myJSONs.size());
    for(size_t i = 0; i < myJSONs.size(); i++)
      (*myJSONsPtr)[i] = myJSONs[i];
  }

  if(otherJSONsPtr)
  {
    create_array(*otherJSONsPtr, otherJSONs.size());
    for(size_t i = 0; i < otherJSONs.size(); i++)
      (*otherJSONsPtr)[i] = otherJSONs[i];
  }

  return ret;
}

extern "C" RENDERDOC_API void RENDERDOC_CC RENDERDOC_UpdateVulkanLayerRegistration(bool systemLevel)
{
  RenderDoc::Inst().UpdateVulkanLayerRegistration(systemLevel);
}