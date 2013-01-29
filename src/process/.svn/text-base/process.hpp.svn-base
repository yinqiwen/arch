/*
 * process.hpp
 *
 *  Created on: 2010-9-27
 *      Author: wqy
 */

#ifndef NOVA_PROCESS_HPP_
#define NOVA_PROCESS_HPP_
#include "common/base.hpp"
#include "process/process_runner.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <vector>
#include <algorithm>

using std::vector;
using std::pair;

namespace arch
{
    namespace process
    {
        /**
         * 进程封装类
         */
        class Process
        {
            private:
                pid_t m_pid;
                vector<pid_t> m_childrens;
                static Process m_currentProcessInstance;
                Process(int pid);
            public:
                Process();
                static Process& CurrentProcess();
                vector<pair<Process, ProcessRunner*> > ForkChildren(
                        const vector<ProcessRunner*>& childRunners,
                        bool asDameon = true);
                /**
                 *  fork一个子进程， 并回调到runner对象的Execute方法
                 *  @asDameon 是否设置当前进程为daemon进程
                 */
                Process Fork(ProcessRunner* runner, bool asDameon = true);
                pid_t GetPID()
                {
                    return m_pid;
                }
                vector<pid_t> GetChildren()
                {
                    return m_childrens;
                }
        };
    }
}

#endif /* PROCESS_HPP_ */
