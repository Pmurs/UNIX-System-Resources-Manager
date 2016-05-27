// Class declarations for an object that represents a running process

#ifndef PROCESSINFO_HXX
#define PROCESSINFO_HXX

#include <cstddef>

class ProcessInfo
{
public:
  ProcessInfo(int pid, int memSize);

  ProcessInfo(const ProcessInfo& rhs);

  const ProcessInfo& operator=(const ProcessInfo& rhs);

  ~ProcessInfo();

  int getPid() const;
  std::string getName() const;
  int getThreads() const;
  int getMemUsage() const;
  double getPercentMemUsage() const;
  double getCpuUsage() const;

private:
  void setCpuUsage();

  int myPid;
  std::string myName;
  int numThreads;
  int memUsage;
  double percentMemUsage;
  double cpuUsage;

};


#endif
