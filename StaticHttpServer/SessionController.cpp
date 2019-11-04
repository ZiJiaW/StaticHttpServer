#include "pch.h"
#include "SessionController.h"

namespace http {

void SessionController::Start(std::shared_ptr<Session> sess)
{
    sessions_.insert(sess);
    sess->Open();
}

void SessionController::Stop(std::shared_ptr<Session> sess)
{
    sessions_.erase(sess);
    sess->Close();
}

void SessionController::StopAll()
{
    for (auto sess : sessions_) {
        sess->Close();
    }
    sessions_.clear();
}

}
