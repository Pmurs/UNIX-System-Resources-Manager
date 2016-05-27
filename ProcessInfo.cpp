// Class definitions for an object that represents a running process

#include <cstddef>
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <unistd.h>
#include "ProcessInfo.hxx"
using namespace std;


// helper function to get the current time of the CPU
double getCpuTime()
{
  int cpuTime = 0.0;

  std::ifstream myFile("/proc/stat");
  std::string word;
  if(myFile.is_open());
  {
    while (myFile>>word)
    {
      if(word == "cpu")
      {
        for (size_t i = 0; i < 10; i++)
        {        
          myFile>>word;
          cpuTime += stoi(word);
        }
        break;
      }
    }
    myFile.close();
  }
  return (double)cpuTime/(double)sysconf(_SC_CLK_TCK);
}


// creates a ProcessInfo object from the given PID
ProcessInfo::ProcessInfo(int pid, int memSize): myPid(pid)
{
  memUsage = 0;
  ifstream myFile("/proc/" + to_string(myPid) + "/status");
  string word;
  if(myFile.is_open());
  {
    while (myFile>>word)
    {
      if(word == "Name:")
      {
        myFile>>word;
        myName = word;
      }
      if(word == "VmPeak:")
      {
        myFile>>word;
        memUsage = stoi(word);
      }
      if(word == "Threads:")
      {
        myFile>>word;
        numThreads = stoi(word);
        break; // got all values we need, break out of loop
      }
    }
    myFile.close();
  }
  if (memUsage != 0)
  {
    percentMemUsage = ((double)memUsage/(double)memSize) * 100;
    setCpuUsage();
  }
  else
    percentMemUsage = 0.0;

  // setCpuUsage();


}

ProcessInfo::ProcessInfo(const ProcessInfo& rhs): myPid(rhs.myPid), myName(rhs.myName), numThreads(rhs.numThreads), memUsage(rhs.memUsage), percentMemUsage(rhs.percentMemUsage), cpuUsage(rhs.cpuUsage)
{
  // nothing to do here
}

ProcessInfo::~ProcessInfo()
{
  // nothing to do here
}

const ProcessInfo& ProcessInfo::operator=(const ProcessInfo& rhs)
{
  if (this != &rhs)
  {
    ProcessInfo tmp(rhs);
    std::swap(myPid, tmp.myPid);
    std::swap(myName, tmp.myName);
    std::swap(numThreads, tmp.numThreads);
    std::swap(memUsage, tmp.memUsage);
    std::swap(percentMemUsage, tmp.percentMemUsage);
    std::swap(cpuUsage, tmp.cpuUsage);
  }
  return *this;
}

int ProcessInfo::getPid() const
{
  return myPid;
}

string ProcessInfo::getName() const
{
  return myName;
}

int ProcessInfo::getThreads() const
{
  return numThreads;
}

int ProcessInfo::getMemUsage() const
{
  return memUsage;
}

double ProcessInfo::getPercentMemUsage() const
{
  return percentMemUsage;
}

double ProcessInfo::getCpuUsage() const
{
  return cpuUsage;
}

// helper function to find out what percentage of the CPU this process is using
void ProcessInfo::setCpuUsage()
{
  double clockRate = (double)sysconf(_SC_CLK_TCK);
  double cpuTimePre = getCpuTime();
  double utimePre;
  double stimePre;
  double utimePost;
  double stimePost;

  ifstream myFile("/proc/" + to_string(myPid) + "/stat");
  string word;
  if(myFile.is_open());
  {
    for (size_t i = 0; i < 13; i++)
    {        
      myFile>>word;
    }
    myFile>>word;
    utimePre = ((double)atoi(word.c_str()))/clockRate;

    myFile>>word;
    stimePre = ((double)atoi(word.c_str()))/clockRate;

    myFile.close();
  }

  this_thread::sleep_for(std::chrono::milliseconds(500));

  double cpuTimePost = getCpuTime();

  ifstream myFile2("/proc/" + to_string(myPid) + "/stat");
  string word2;
  if(myFile2.is_open());
  {
    for (size_t i = 0; i < 13; i++)
    {        
      myFile2>>word2;
    }
    myFile2>>word2;
    utimePost = ((double)atoi(word2.c_str()))/clockRate;

    myFile2>>word2;
    stimePost = ((double)atoi(word2.c_str()))/clockRate;

    myFile2.close();
  }

  double user_util = 100 * (utimePost - utimePre)/(cpuTimePost - cpuTimePre);
  double system_util = 100 * (stimePost - stimePre)/(cpuTimePost - cpuTimePre);

  cout << "Working...\n";

  cpuUsage = user_util + system_util;
}
