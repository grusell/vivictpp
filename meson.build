project('vivict++', 'cpp', default_options : ['cpp_std=c++17', 'warning_level=3'])

#if meson.get_compiler('cpp').get_id() == 'gcc'
#  extra_args=['-Wno-shadow', '-Wno-declaration-after-statement']
#else
  extra_args=[]
#endif

fmt_proj = subproject('fmt', default_options: ['warning_level=0', 'default_library=static'])  
spdlog_proj = subproject('spdlog', default_options: 'warning_level=0')
cli11_proj = subproject('cli11', default_options: 'warning_level=0')
catch2_proj = subproject('catch2', default_options: 'warning_level=0')
freetype2_proj = subproject('freetype2', default_options: 'warning_level=0')
zlib_proj = subproject('zlib', default_options: 'warning_level=0')

cc = meson.get_compiler('cpp')
libavformat_dep = cc.find_library('avformat', static: true,
               dirs : ['C:\Program Files\FFmpeg\lib'])
libavcodec_dep = cc.find_library('avcodec', static: true,
               dirs : ['C:\Program Files\FFmpeg\lib'])
libavutil_dep = cc.find_library('avutil', static: true,
               dirs : ['C:\Program Files\FFmpeg\lib'])
libavfilter_dep = cc.find_library('avfilter', static: true,
               dirs : ['C:\Program Files\FFmpeg\lib'])
libswscale_dep = cc.find_library('swscale', static: true,
               dirs : ['C:\Program Files\FFmpeg\lib'])
	       

ffmpeg_inc = include_directories('C:\Program Files\FFmpeg\include')
#ffmpeg_lib = static_library('C:\Program Files\FFmpeg\lib\libavformat.dll.a')
#ffmpeg_dep = declare_dependency(link_with: ffmpeg_lib, include_directories: ffmpeg_inc)

if get_option('use_sdl2_subproject')
  sdl2_proj = subproject('sdl2', default_options: ['warning_level=0', 'test=false', 'default_library=static'])
  sdl2_dep = sdl2_proj.get_variable('sdl2_dep')
  sdl2_ttf_proj = subproject('sdl2_ttf', default_options: ['warning_level=0', 'default_library=static'])
  sdl2_ttf_dep = sdl2_ttf_proj.get_variable('sdl2_ttf_dep')
else
  sdl2_dep = dependency('sdl2', fallback: ['sdl2', 'sdl2_dep'])
  sdl2_ttf_dep = dependency('sdl2_ttf', fallback: ['sdl2_ttf', 'sdl2_ttf_dep'])
endif

deps = [
  sdl2_dep,
  sdl2_ttf_dep,
#  dependency('freetype2', version: '>=2.8.1'),
  zlib_proj.get_variable('zlib_dep'),
#  freetype2_proj.get_variable('freetype2_dep'),
  dependency('threads'),
#  dependency('libavformat'),
   libavformat_dep,
   libavcodec_dep,
   libavutil_dep,
   libavfilter_dep,
   libswscale_dep,	
#  dependency('libavcodec'),
#  dependency('libavutil'),
#  dependency('libavfilter'),
#  dependency('libswscale'),
  spdlog_proj.get_variable('spdlog_dep'),
  cli11_proj.get_variable('CLI11_dep'),
  fmt_proj.get_variable('fmt_dep')
]

test_deps = [
  catch2_proj.get_variable('catch2_dep')
]

incdir = include_directories('include', 'C:\Program Files\FFmpeg\include')

git_hash_cmd = run_command('git', 'rev-parse', '--short', 'HEAD')
git_hash = git_hash_cmd.stdout().strip()

sources = [
  'src/AVSync.cc',
  'src/Controller.cc',
  'src/VideoInputs.cc',
  'src/VideoMetadata.cc',
  'src/VivictPP.cc',
  'src/libav/Decoder.cc',
  'src/libav/Filter.cc',
  'src/libav/FormatHandler.cc',
  'src/libav/Frame.cc',
  'src/libav/HwAccelUtils.cc',
  'src/libav/Packet.cc',
  'src/libav/Utils.cc',
  'src/logging/Logging.cc',
  'src/sdl/SDLAudioOutput.cc',
  'src/sdl/SDLEventLoop.cc',
  'src/sdl/SDLUtils.cc',
  'src/sdl/SDLUtils.cc',
  'src/time/TimeUtils.cc',
  'src/ui/Container.cc',
  'src/ui/FontSize.cc',
  'src/ui/Fonts.cc',
  'src/ui/MetadataDisplay.cc',
  'src/ui/ScreenOutput.cc',
  'src/ui/SeekBar.cc',
  'src/ui/TextBox.cc',
  'src/ui/TextTexture.cc',
  'src/ui/VideoDisplay.cc',
  'src/ui/VmafGraph.cc',
  'src/vmaf/VmafLog.cc',
  'src/workers/DecoderWorker.cc',
  'src/workers/FrameBuffer.cc',
  'src/workers/PacketQueue.cc',
  'src/workers/PacketWorker.cc',
  'src/workers/QueuePointer.cc',
  'src/workers/VideoInputMessage.cc',
]

vivictpplib = static_library('vivictpplib',
                             sources: sources,
                             dependencies: deps,
                             include_directories: incdir,
                             cpp_args: extra_args)

vpp_extra_args = extra_args
vpp_extra_args += '-DVPP_GIT_HASH="' + git_hash + '"'

executable('vivictpp', 'src/main.cc', link_with: vivictpplib, dependencies: deps,
           include_directories: incdir, install: true, cpp_args : vpp_extra_args, link_args: '-g')

#seekTest= executable('seekTest', 'test/SeekTest.cc', link_with: vivictpplib,  dependencies: deps + test_deps, include_directories: incdir)
# test('FormatHandler.seek', seekTest)
playbackTest= executable('playbackTest', 'test/PlaybackTest.cc', link_with: vivictpplib,  dependencies: deps + test_deps, include_directories: incdir, cpp_args: extra_args)
test('Playback', playbackTest)
