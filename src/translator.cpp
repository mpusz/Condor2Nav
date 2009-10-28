#include "translator.h"
#include "tools.h"


condor2nav::CTranslator::CTranslator(const std::string &outputPath):
_outputPath(outputPath)
{
  DirectoryCreate(_outputPath);
}


condor2nav::CTranslator::~CTranslator()
{
}


const std::string &condor2nav::CTranslator::OutputPath() const
{
  return _outputPath;
}
