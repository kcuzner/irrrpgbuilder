#ifndef XMLMANAGER_H
#define XMLMANAGER_H


class XMLManager
{
    public:
        static XMLManager* getInstance();
        virtual ~XMLManager();

        bool saveProject();
    protected:
    private:
        XMLManager();
};

#endif // XMLMANAGER_H
