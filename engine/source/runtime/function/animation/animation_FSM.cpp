#include "runtime/function/animation/animation_FSM.h"
#include <iostream>
namespace Pilot
{
    AnimationFSM::AnimationFSM() {}
    float tryGetFloat(const json11::Json::object& json, const std::string& key, float default_value)
    {
        auto found_iter = json.find(key);
        if (found_iter != json.end() && found_iter->second.is_number())
        {
            return found_iter->second.number_value();
        }
        return default_value;
    }
    bool tryGetBool(const json11::Json::object& json, const std::string& key, float default_value)
    {
        auto found_iter = json.find(key);
        if (found_iter != json.end() && found_iter->second.is_bool())
        {
            return found_iter->second.bool_value();
        }
        return default_value;
    }
    bool AnimationFSM::update(const json11::Json::object& signals)
    {
        States last_state     = m_state;
        bool   is_clip_finish = tryGetBool(signals, "clip_finish", false);
        bool   is_jumping     = tryGetBool(signals, "jumping", false);
        float  speed          = tryGetFloat(signals, "speed", 0);
        bool   is_moving      = speed > 0.01f;
        bool   start_walk_end = false;

        switch (m_state)
        {
            case States::_idle:
            {
                m_state = idleStateUpdate(is_jumping, is_moving);
                //std::cout << "state: idle" << std::endl;
                break;
            }
            case States::_walk_start:
            {
                m_state = walkStartUpdate(is_clip_finish);
                //std::cout << "state: _walk_start" << std::endl;
                break;
            }
            case States::_walk_run: 
            {
                m_state = walkRunUpdate(is_jumping, is_moving, is_clip_finish, start_walk_end);
                //std::cout << "state: _walk_run" << std::endl;
                break;
            }
            case States::_walk_stop: 
            {
                m_state = walkEndUpdate(is_moving, is_clip_finish);
                //std::cout << "state: _walk_stop" << std::endl;
                break;
            }
            case States::_jump_start_from_idle: 
            {
                m_state = jumpStartFromIdleUpdate(is_clip_finish);
                //std::cout << "state: _jump_start_from_idle" << std::endl;
                break;
            }
            case States::_jump_loop_from_idle: 
            {
                m_state = jumpLoopFromIdleUpdate(is_jumping);
                //std::cout << "state: _jump_loop_from_idle" << std::endl;
                break;
            }
            case States::_jump_end_from_idle: 
            {
                m_state = jumpEndFromIdleUpdate(is_clip_finish);
                //std::cout << "state: _jump_end_from_idle" << std::endl;
                break;
            }
            case States::_jump_start_from_walk_run: 
            {
                m_state = jumpStartFromWalkRunUpdate(is_clip_finish);
                //std::cout << "state: _jump_start_from_walk_run" << std::endl;
                break;
            }
            case States::_jump_loop_from_walk_run: 
            {
                m_state = jumpLoopFromWalkRunUpdate(is_jumping);
                //std::cout << "state: _jump_loop_from_walk_run" << std::endl;
                break;
            }
            case States::_jump_end_from_walk_run: 
            {
                m_state = jumpEndFromWalkRunUpdate(is_clip_finish);
                //std::cout << "state: _jump_end_from_walk_run" << std::endl;
                break;
            }
            default:
                break;
        }
        return last_state != m_state;
    }

    std::string AnimationFSM::getCurrentClipBaseName() const
    {
        switch (m_state)
        {
            case States::_idle:
                return "idle_walk_run";
            case States::_walk_start:
                return "walk_start";
            case States::_walk_run:
                return "idle_walk_run";
            case States::_walk_stop:
                return "walk_stop";
            case States::_jump_start_from_walk_run:
            case States::_jump_start_from_idle:
                return "jump_start";
            case States::_jump_loop_from_walk_run:
            case States::_jump_loop_from_idle:
                return "jump_loop";
            case States::_jump_end_from_walk_run:
            case States::_jump_end_from_idle:
                return "jump_stop";
            default:
                return "idle_walk_run";
        }
    }

    AnimationFSM::States AnimationFSM::idleStateUpdate(bool is_jumping, bool is_moving) const
    {
        if (is_jumping)
            return States::_jump_start_from_idle;
        if (is_moving)
            return States::_walk_start;
        return States::_idle;
    }
    AnimationFSM::States AnimationFSM::walkStartUpdate(bool is_clip_finish) const 
    {
        if (is_clip_finish)
            return States::_walk_run;
        return States::_walk_start;
    }
    AnimationFSM::States AnimationFSM::walkRunUpdate(bool is_jumping, bool is_moving, bool is_clip_finish, bool start_walk_end) const
    {
        if (is_jumping)
            return States::_jump_start_from_walk_run;
        if (is_clip_finish && start_walk_end)
            return States::_walk_stop;
        if (!is_moving)
            return States::_idle;
        
        return States::_walk_run;
    }
    AnimationFSM::States AnimationFSM::walkEndUpdate(bool is_moving, bool is_clip_finish) const 
    { 
        if (!is_moving && is_clip_finish)
            return States::_idle; 
        return States::_walk_stop;
    }
    AnimationFSM::States AnimationFSM::jumpStartFromIdleUpdate(bool is_clip_finish) const 
    { 
        if (is_clip_finish)
            return States::_jump_loop_from_idle;
        return States::_jump_start_from_idle; 
    }
    AnimationFSM::States AnimationFSM::jumpLoopFromIdleUpdate(bool is_jumping) const 
    {
        if (!is_jumping)
            return States::_jump_end_from_idle;
        return States::_jump_loop_from_idle; 
    }
    AnimationFSM::States AnimationFSM::jumpEndFromIdleUpdate(bool is_clip_finish) const 
    {
        if (is_clip_finish)
            return States::_idle;
        return States::_jump_end_from_idle; 
    }
    AnimationFSM::States AnimationFSM::jumpStartFromWalkRunUpdate(bool is_clip_finish) const 
    {
        if (is_clip_finish)
            return States::_jump_loop_from_walk_run;
        return States::_jump_start_from_walk_run; 
    }
    AnimationFSM::States AnimationFSM::jumpLoopFromWalkRunUpdate(bool is_jumping) const 
    { 
        if (!is_jumping)
            return States::_jump_end_from_walk_run;
        return States::_jump_loop_from_walk_run; 
    }
    AnimationFSM::States AnimationFSM::jumpEndFromWalkRunUpdate(bool is_clip_finish) const 
    { 
        if (is_clip_finish)
            return States::_walk_run;
        return States::_jump_end_from_walk_run; 
    }
}

