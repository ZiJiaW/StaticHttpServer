#pragma once
#include "pch.h"
#include "SessionController.h"
#include "Session.h"

namespace http {

class SessionController
{
public:

    SessionController(const SessionController &sc) = delete;
    SessionController operator= (const SessionController &sc) = delete;
    SessionController() = default;
    ~SessionController() = default;

    void StopAll();
    void Stop(std::shared_ptr<Session> sp);
    void Start(std::shared_ptr<Session> sp);

private:
    std::set<std::shared_ptr<Session>> sessions_;
};

}
