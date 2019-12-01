#include "pch.h"
#include "SessionController.h"

namespace http {

void SessionController::Start(std::shared_ptr<Session> sess)
{
    sess->Open();
    //std::lock_guard<std::mutex> lg(sessions_lock);
    //sessions_.insert(sess);
}

void SessionController::Stop(std::shared_ptr<Session> sess)
{
    sess->Close();
    //sessions_.erase(sess);
}

void SessionController::StopAll()
{
    for (auto sess : sessions_) {
        sess->Close();
    }
    sessions_.clear();
}

}
