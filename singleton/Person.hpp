#pragma once
#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>
using namespace std;

class Person
{
public:
    Person(string name = "", int age = 20)
        : m_name(name), m_age(age)
    {}
    void eat(string what)
    {
        cout << m_name << " " << "i am eating : " << what << endl;
    }
private:
    string m_name;
    int m_age;
};

class Singleton
{
public:
    static Person* get_instance()
    {
        /*
        if (m_p == nullptr)
        {
            pthread_mutex_lock(&m_lock);
            if (m_p == nullptr)
            {
                cout << "first use, create object!" << endl;
                sleep(3);
                m_p = new Person("zhangsan", 23);
                cout << "address: " << m_p << endl;
                sleep(10);
            }
            else
            {
                cout << "object is exist! retuen current object" << endl;
            }
            pthread_mutex_unlock(&m_lock);
        }
        */
        return m_p;
    }
private:
    static Person* m_p;
    //static Person  *volatile m_p;
    //static pthread_mutex_t m_lock;
};

//Person *volatile Singleton::m_p = nullptr;
//pthread_mutex_t Singleton::m_lock = PTHREAD_MUTEX_INITIALIZER;
Person* Singleton::m_p = new Person("zhangsan", 23);




