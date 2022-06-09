from ctypes.wintypes import LARGE_INTEGER
from multiprocessing.dummy import current_process
import os
import sys 
import string
import shutil

inBuildDir = None
inMediaDir = None
inConfigFilePath = None

for arg in sys.argv[1:]:
        splitArgument = arg.split('=')
        if splitArgument[0].lower() == "builddir":
            inBuildDir = splitArgument[1]
        elif splitArgument[0].lower() == "mediadir":
            inMediaDir = splitArgument[1]
        elif splitArgument[0].lower() == "configfile":
            inConfigFilePath = splitArgument[1]
        else:
            print("Unknown argument: " + splitArgument[0])

if inMediaDir is None:
    print("Source media directory was not provided")
    exit()
if inBuildDir is None:
    print("Build directory was not provided")
    exit()

destinationMediaDir = os.path.join(inBuildDir, "media")
destinationConfigFile = os.path.join(inBuildDir, os.path.basename(inConfigFilePath))

def CopyFile(sourceDir : string, destinationDir : string):
    fileName = os.path.basename(sourceDir)
    if not os.path.exists(destinationDir):
        os.makedirs(destinationDir)
    finalDestination = os.path.join(destinationDir, fileName)
    shutil.copy2(sourceDir, finalDestination)

#copy media as needed 
def SearchForFiles(currentSourcePath : string, currentDestPath : string):
    dirList = os.listdir(currentSourcePath)
    for dir in dirList:
        fullSourcePath = os.path.join(currentSourcePath, dir)
        if os.path.isdir(fullSourcePath):
            #is dir need to go deeper
            fullDestinationPath = os.path.join(currentDestPath, dir)
            SearchForFiles(fullSourcePath, fullDestinationPath)
        else:
            #is file need to copy
            print(fullSourcePath + " - " + currentDestPath )
            CopyFile(fullSourcePath, currentDestPath)

SearchForFiles(inMediaDir, destinationMediaDir)

#update config file
lines = []
if os.path.isfile(inConfigFilePath):
    with open(inConfigFilePath, 'r') as f:
        lines = f.readlines()
print(destinationConfigFile)
if os.path.isfile(destinationConfigFile) is False:
    print("Creating config file")
    with open(destinationConfigFile, 'x') as f:
        for line in lines:
            setting = line.split('=')
            if (setting[0].lower() == "mediadirectory"):
                setting[1] = "./media/"
            line = setting[0] + "=" + setting[1]
            f.write(line)