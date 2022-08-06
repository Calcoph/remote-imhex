#include <hex/plugin.hpp>

#include <hex/api/event.hpp>
#include <thread>
#include <string>
#include <iostream>
#include <sys/ioctl.h>
#include <queue>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;


int port_listen(mutex &m, queue<string> &q);

#include <hex/api/content_registry.hpp>
#include <hex/ui/view.hpp>

class ViewExample : public hex::View {
private:
	thread* t_handler;
    queue<string> q;
    mutex m;
public:
    ViewExample() : hex::View("Example") {
		t_handler = new thread(port_listen, ref(m), ref(q));
	}
    ~ViewExample() override = default;

    void drawContent() override {
        if (ImGui::Begin("Example")) {
            ImGui::Text("Custom plugin window");
        }
        m.lock();
        if (!q.empty()) {
            string str = q.front();
            q.pop();
            hex::EventManager::post<hex::RequestSetPatternLanguageCode>(str);
        }
        m.unlock();
        ImGui::End();
    }
};

IMHEX_PLUGIN_SETUP("C++ Template Plugin", "Plugin Author", "Plugin Description") {
    //hex::EventManager::post<hex::RequestSetPatternLanguageCode>("Hello World");
	hex::ContentRegistry::Views::add<ViewExample>();
}

string convertToString(char* a, int size)
{
    int i;
    string s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s;
}

int port_listen(mutex &m, queue<string> &q) {
    int socketDesc;
    int opt = 1;
    struct  sockaddr_in server;
    string message;
    int port = 5757;
    cout << "The port is " << port << endl;
    socketDesc = socket(AF_INET, SOCK_STREAM, 0);
    if(socketDesc == -1){
            cout << "ERROR CREATING SOCKET DESCRIPTOR" << endl;
            exit(EXIT_FAILURE);
    }

    if(setsockopt(socketDesc, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
            cout << "Setsocket error" << endl;
            exit(EXIT_FAILURE);
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_port = htons(port);

    if(bind(socketDesc, (struct sockaddr *)&server, sizeof(server)) < 0){
            cout << "BIND FAILED" << endl;
            exit(EXIT_FAILURE);
    }
    cout << "Bind finished" << endl; //Page 982

    if(listen(socketDesc,1) < 0){
        char buffer[1024];
        int len = 0;
        while (true) {
            m.lock();
            if (q.empty()) {
                m.unlock();
                ioctl(socketDesc, FIONREAD, &len);
                if (len > 0) {
                    string str;
                    while (len > 0) {
                        ioctl(socketDesc, FIONREAD, &len);
                        len = read(socketDesc, buffer, len);
                        str = str + convertToString(buffer, len / sizeof(char));
                    }
                    m.lock();
                    q.push(str);
                    m.unlock();
                };
            }
        }
    }

    return socketDesc;
}
