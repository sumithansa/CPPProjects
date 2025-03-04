#include<iostream>
#include<string>

class Log
{
    public:
        enum Level
        {
            LevelError = 0, LevelWarning, LevelInfo
        };
    private:
        int& m_LogLevel;
    public:
        Log(int& Info):m_LogLevel(Info){}
        int GetLevel()const
        {
            return m_LogLevel;
        }
        void SetLevel(Level level)
        {
            m_LogLevel = level;
        }

        void error(const char* message)
        {
            if(m_LogLevel >= LevelError)
                std::cout<<"[Error]: "<<message<<std::endl;
        }
        void warn(const char* message)
        {
            if(m_LogLevel >= LevelWarning)
                std::cout<<"[Warning]: "<<message<<std::endl;
        }
        void Info(const char* message)
        {
            if(m_LogLevel >= LevelInfo)
                std::cout<<"[Info]: "<<message<<std::endl;
        }

};