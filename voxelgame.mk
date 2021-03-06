##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=voxelgame
ConfigurationName      :=Debug
WorkspacePath          :=/home/jacek/cprograms/minestruggle
ProjectPath            :=/home/jacek/cprograms/minestruggle/minestruggle
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=Jacek
Date                   :=06/03/21
CodeLitePath           :=/home/jacek/.codelite
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/$(ProjectName)
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="voxelgame.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)./include 
IncludePCH             := 
RcIncludePath          := 
Libs                   := $(LibrarySwitch)glfw3 $(LibrarySwitch)GL $(LibrarySwitch)X11 $(LibrarySwitch)Xxf86vm $(LibrarySwitch)Xrandr $(LibrarySwitch)pthread $(LibrarySwitch)Xi $(LibrarySwitch)dl 
ArLibs                 :=  "glfw3" "GL" "X11" "Xxf86vm" "Xrandr" "pthread" "Xi" "dl" 
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS :=  -g -O0 -Wall -std=c++17  -Lglfw3.so $(Preprocessors)
CFLAGS   :=  -g -O0 -Wall $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
Objects0=$(IntermediateDirectory)/src_utilities_fonts.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_terrain_WorldLoader.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_objects_Mesh.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_glad.c$(ObjectSuffix) $(IntermediateDirectory)/src_utilities_basic_util.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_objects_Cursor.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_utilities_controls.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_terrain_Terrain.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_terrain_Region.cpp$(ObjectSuffix) \
	$(IntermediateDirectory)/src_terrain_Chunk.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(OutputFile)

$(OutputFile): $(IntermediateDirectory)/.d $(Objects) 
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(LinkerName) $(OutputSwitch)$(OutputFile) @$(ObjectsFileList) $(LibPath) $(Libs) $(LinkOptions)

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


$(IntermediateDirectory)/.d:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/src_utilities_fonts.cpp$(ObjectSuffix): src/utilities/fonts.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_utilities_fonts.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_utilities_fonts.cpp$(DependSuffix) -MM src/utilities/fonts.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/jacek/cprograms/minestruggle/minestruggle/src/utilities/fonts.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_utilities_fonts.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_utilities_fonts.cpp$(PreprocessSuffix): src/utilities/fonts.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_utilities_fonts.cpp$(PreprocessSuffix) src/utilities/fonts.cpp

$(IntermediateDirectory)/src_terrain_WorldLoader.cpp$(ObjectSuffix): src/terrain/WorldLoader.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_terrain_WorldLoader.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_terrain_WorldLoader.cpp$(DependSuffix) -MM src/terrain/WorldLoader.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/jacek/cprograms/minestruggle/minestruggle/src/terrain/WorldLoader.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_terrain_WorldLoader.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_terrain_WorldLoader.cpp$(PreprocessSuffix): src/terrain/WorldLoader.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_terrain_WorldLoader.cpp$(PreprocessSuffix) src/terrain/WorldLoader.cpp

$(IntermediateDirectory)/src_objects_Mesh.cpp$(ObjectSuffix): src/objects/Mesh.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_objects_Mesh.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_objects_Mesh.cpp$(DependSuffix) -MM src/objects/Mesh.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/jacek/cprograms/minestruggle/minestruggle/src/objects/Mesh.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_objects_Mesh.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_objects_Mesh.cpp$(PreprocessSuffix): src/objects/Mesh.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_objects_Mesh.cpp$(PreprocessSuffix) src/objects/Mesh.cpp

$(IntermediateDirectory)/src_glad.c$(ObjectSuffix): src/glad.c
	@$(CC) $(CFLAGS) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_glad.c$(ObjectSuffix) -MF$(IntermediateDirectory)/src_glad.c$(DependSuffix) -MM src/glad.c
	$(CC) $(SourceSwitch) "/home/jacek/cprograms/minestruggle/minestruggle/src/glad.c" $(CFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_glad.c$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_glad.c$(PreprocessSuffix): src/glad.c
	$(CC) $(CFLAGS) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_glad.c$(PreprocessSuffix) src/glad.c

$(IntermediateDirectory)/src_utilities_basic_util.cpp$(ObjectSuffix): src/utilities/basic_util.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_utilities_basic_util.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_utilities_basic_util.cpp$(DependSuffix) -MM src/utilities/basic_util.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/jacek/cprograms/minestruggle/minestruggle/src/utilities/basic_util.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_utilities_basic_util.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_utilities_basic_util.cpp$(PreprocessSuffix): src/utilities/basic_util.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_utilities_basic_util.cpp$(PreprocessSuffix) src/utilities/basic_util.cpp

$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix): src/main.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_main.cpp$(DependSuffix) -MM src/main.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/jacek/cprograms/minestruggle/minestruggle/src/main.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_main.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix): src/main.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_main.cpp$(PreprocessSuffix) src/main.cpp

$(IntermediateDirectory)/src_objects_Cursor.cpp$(ObjectSuffix): src/objects/Cursor.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_objects_Cursor.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_objects_Cursor.cpp$(DependSuffix) -MM src/objects/Cursor.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/jacek/cprograms/minestruggle/minestruggle/src/objects/Cursor.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_objects_Cursor.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_objects_Cursor.cpp$(PreprocessSuffix): src/objects/Cursor.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_objects_Cursor.cpp$(PreprocessSuffix) src/objects/Cursor.cpp

$(IntermediateDirectory)/src_utilities_controls.cpp$(ObjectSuffix): src/utilities/controls.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_utilities_controls.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_utilities_controls.cpp$(DependSuffix) -MM src/utilities/controls.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/jacek/cprograms/minestruggle/minestruggle/src/utilities/controls.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_utilities_controls.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_utilities_controls.cpp$(PreprocessSuffix): src/utilities/controls.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_utilities_controls.cpp$(PreprocessSuffix) src/utilities/controls.cpp

$(IntermediateDirectory)/src_terrain_Terrain.cpp$(ObjectSuffix): src/terrain/Terrain.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_terrain_Terrain.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_terrain_Terrain.cpp$(DependSuffix) -MM src/terrain/Terrain.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/jacek/cprograms/minestruggle/minestruggle/src/terrain/Terrain.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_terrain_Terrain.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_terrain_Terrain.cpp$(PreprocessSuffix): src/terrain/Terrain.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_terrain_Terrain.cpp$(PreprocessSuffix) src/terrain/Terrain.cpp

$(IntermediateDirectory)/src_terrain_Region.cpp$(ObjectSuffix): src/terrain/Region.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_terrain_Region.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_terrain_Region.cpp$(DependSuffix) -MM src/terrain/Region.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/jacek/cprograms/minestruggle/minestruggle/src/terrain/Region.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_terrain_Region.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_terrain_Region.cpp$(PreprocessSuffix): src/terrain/Region.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_terrain_Region.cpp$(PreprocessSuffix) src/terrain/Region.cpp

$(IntermediateDirectory)/src_terrain_Chunk.cpp$(ObjectSuffix): src/terrain/Chunk.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_terrain_Chunk.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_terrain_Chunk.cpp$(DependSuffix) -MM src/terrain/Chunk.cpp
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/jacek/cprograms/minestruggle/minestruggle/src/terrain/Chunk.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_terrain_Chunk.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_terrain_Chunk.cpp$(PreprocessSuffix): src/terrain/Chunk.cpp
	$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_terrain_Chunk.cpp$(PreprocessSuffix) src/terrain/Chunk.cpp


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


