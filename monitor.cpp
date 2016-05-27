/*
Paul Gelston Murray
CS 281
Final Project
System Resources Monitor
*/

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <thread>
#include <cstdio>
#include <dirent.h>
#include <boost/program_options/option.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include "ProcessInfoList.hxx"



// struct to hold information about the system's memory
struct MemValues
{
  int memTotal;
  int memFree;
  double percentMemFree;
  double percentMemUsed;
};

// helper function to get information about the system's memory
MemValues getMemSize()
{

  MemValues myValues;

  std::ifstream myFile("/proc/meminfo");
  std::string word;
  if(myFile.is_open());
  {
    while (myFile>>word)
    {
      if(word == "MemTotal:")
      {
        myFile>>word;
        myValues.memTotal = std::stoi(word);
      }
      if(word == "MemFree:")
      {
        myFile>>word;
        myValues.memFree = std::stoi(word);
        break; // got both values we need, break out of loop
      }
    }
    myFile.close();
  }
  
  myValues.percentMemFree = ((double)myValues.memFree / (double)myValues.memTotal) * 100;
  myValues.percentMemUsed = 100.0 - myValues.percentMemFree;
  return myValues;
}



// This is where all the real work is done
void auxillary (size_t processNum, size_t reportNum, size_t pollingInterval)
{
  while (reportNum != 0)
  {

    struct dirent *pDirent;
    DIR *pDir;

    size_t count = 0;
    size_t numProcesses = 0;
    double cpuUtil = 0.0;

    ProcessInfoList myList;
    MemValues myValues = getMemSize();

    pDir = opendir("/proc");
    if (pDir == NULL)
    {
      printf("Cannot open /proc directory.");
      return;
    }

    while ((pDirent = readdir(pDir)) != NULL)
    {
      if (count >= 63) // terrible terrible hardcoding
      {
//      printf ("[%s]\n", pDirent->d_name);
        ProcessInfo x(atoi(pDirent->d_name), myValues.memTotal);
        myList.listAdd(x);
        cpuUtil += x.getCpuUsage();
        numProcesses += 1;
      }
      count++;
    }
    closedir(pDir);

    int processNumbers;
    if (processNum > numProcesses)
        processNumbers = numProcesses;
    else
        processNumbers = processNum;

    std::cout << "\n";

    // do work to report system information
    std::cout << "Total CPU Utilization:" << cpuUtil << "%\n";

    std::cout << "Total System RAM: " << myValues.memTotal << " KB;\tFree Memory Available: " << myValues.memFree << " KB, \t" << myValues.percentMemFree << "%;\tMemory In Use: " << myValues.percentMemUsed << "%\n\n";

    for (int i = 0; i < processNumbers; i++)
    {
      ProcessInfo head = myList.listFront();
      std::string extraTab, extraTab2 = "";
      if (head.getName().length() <= 8)
          extraTab = "\t";
      if (head.getPid() <= 9)
          extraTab2 = "\t";

      std::cout << "Name: " << head.getName() << ";" << extraTab << "\tPID: " << head.getPid() << ";" << extraTab2 << "\tThread Count: " << head.getThreads() << ";\tMemory Usage: " << head.getMemUsage() << " KB;\tPercent Memory Usage: " << head.getPercentMemUsage() << "%;\tCPU Usage: " << head.getCpuUsage() << "%\n";
      myList.listDequeue();
    }
    std::cout << "\n";

    // do work to wait pollingInterval seconds before running again
    std::this_thread::sleep_for(std::chrono::seconds(pollingInterval));
    reportNum--;
  }
}

namespace po = boost::program_options;
po::variables_map vm;

int main(int argc, char **argv)
{
  int processNum;
  int reportNum;
  int pollingInterval = 0;

  try
  {
      // Parse program options
      po::options_description desc("Allowed options");
      desc.add_options()
		    ("help,h", "Produce this help message")
		    ("processNum,n", po::value<int>(), "number of process to report on")
		    ("reportNum,r", po::value<int>(), "number of times to report")
		    ("pollingInterval,p", po::value<int>(), "polling interval between reports (in seconds)")
		    ;

      po::store(po::parse_command_line(argc, argv, desc), vm);
      po::notify(vm);

      if (vm.count("help")) {
          std::cout << desc << "\n";
          return 0;
      }

      if (!vm.count ("processNum"))
        {
          std::cerr << "No process number given!\n";
          return -1;
        }
      processNum=vm["processNum"].as < int > ();

      if (!vm.count ("reportNum"))
        {
          std::cerr << "No report number given!\n";
          return -1;
        }
      reportNum=vm["reportNum"].as < int > ();

      if (!vm.count ("pollingInterval"))
        {
          std::cerr << "No polling interval given!\n";
          return -1;
        }
      pollingInterval=vm["pollingInterval"].as < int > ();

  }
  catch (std::exception &ex)
  {
      std::cerr << "Caught fatal exception: " << ex.what () << std::endl;
      return -1;
  }

  auxillary(processNum, reportNum, pollingInterval);

  return 0;
}
