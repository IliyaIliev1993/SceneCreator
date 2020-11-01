//
//  TimerMgr.cpp
//  VirtualCockpit
//
//  Created by Iliya Iliev on 10.02.20.
//  Copyright © 2020 Iliya Iliev. All rights reserved.
//

#include <main_app/timer_mgr/TimerMgr.hpp>
#include <main_app/debug/Log.h>

TimerMgr::TimerMgr()
{
    m_eState = StateOfTimer::eStopped;
    m_nTimes = 0;
}

void TimerMgr::Start(unsigned int nID, unsigned int nPeriod)
{
    if(m_eState == StateOfTimer::eStarted)
    {
        LOG_ERROR("Timer: {nID} already STARTED", nID);
        Stop();
        return;
    }
    
    m_nID = nID;
    m_nPeriod = nPeriod;
    m_Started = glfwGetTime();
    m_eState = StateOfTimer::eStarted;
}

void TimerMgr::Stop()
{
    m_eState = StateOfTimer::eStopped;
    m_nTimes = 0;
}

bool TimerMgr::IsStarted()
{
    return m_eState == StateOfTimer::eStarted;
}

bool TimerMgr::IsStopped()
{
    return m_eState == StateOfTimer::eStopped;
}

void TimerMgr::Tick(std::function<void()>callback)
{
    if(m_eState == StateOfTimer::eStopped)
    {
        return;
    }
    
    m_Elapsed = glfwGetTime() - m_Started;
    if(m_Elapsed * 1000.0 >= m_nPeriod)
    {
        m_Started = glfwGetTime();
        m_nTimes++;
        callback();
    }
}

unsigned int& TimerMgr::GetTimes()
{
    return m_nTimes;
}
