#include "UserThread.h"
#include <QtCore/QDataStream>
#include <string>
#include <assert.h>

UserThread::UserThread(int socketDescriptor, QObject *parent):QThread(parent), socketDescriptor(socketDescriptor) {
    userId = -1;
    ready = false;
    connect(tcpSocket, &QIODevice::readyRead, this, &UserThread::readData);
}

void UserThread::run() {
    if (!tcpSocket.setSocketDescriptor(socketDescriptor)) {
        emit error(tcpSocket.error());
        return;
    }
    in.setDevice(tcpSocket);
    in.setVersion(QDataStream::Qt_4_0);

    while(!ready) ; //busy waiting for data ready, not a good idea
                    //maybe mutex or semaphor is better

    // parse header
    Header h = dp.header();
    // assert(h.type() == Header::REQUEST);
    switch(h.resource()) {
        case Header::LOGIN : {
            const Any& any = dp.body();
            if(any.Is<UserInfo>) { // must be true 
                UserInfo userInfo;
                any.UnpackTo(&userInfo);
                //TBD: process login 
            }
            else 
                assert(0);           
            break;
        }

        case Header::REGISTER : {
            const Any& any = dp.body();
            if(any.Is<UserInfo>) { // must be true 
                UserInfo userInfo;
                any.UnpackTo(&userInfo);
                //TBD: process register
            }
            else 
                assert(0);           
            break;
        }

        case Header::SEARCH_MUSIC : {
            const Any& any = dp.body();
            if(any.Is<MusicInfo>) { // must be true 
                MusicInfo musicInfo;
                any.UnpackTo(&musicInfo);
                //TBD: process search music
            }
            else 
                assert(0);           
            break;
        }

        case Header::SEARCH_USER : {
            const Any& any = dp.body();
            if(any.Is<UserInfo>) { // must be true 
                UserInfo userInfo;
                any.UnpackTo(&userInfo);
                //TBD: process search user
            }
            else 
                assert(0);           
            break;
        }

        case Header::COMMENT = 4 : {
            const Any& any = dp.body();
            if(any.Is<CommentInfo>) { // must be true 
                CommentInfo commentInfo;
                any.UnpackTo(&commentInfo);
                //TBD: process comment
            }
            else 
                assert(0);           
            break;
        }

		case Header::LIKE = 5 : {
            const Any& any = dp.body();
            if(any.Is<CommentInfo>) { // must be true 
                CommentInfo commentInfo;
                any.UnpackTo(&commentInfo);
                //TBD: process like
            }
            else 
                assert(0);           
            break;
        }

		case Header::GET_COMMENTS : {
            //TBD: process get comments
        }

		case Header::GET_MUSICS : {

        }
        
		case Header::GET_MUSICFILE : {

        }

		case Header::GET_USERS : {

        }
    }
}

void readData() {
    in.startTransaction();
    
    in >> data;
    if(!in.commitTransaction())
        return;
    // if(nextData == currentData)
    //     return;

    std::string raw = data.toStdString();
    dp.ParseFromString(data);
    ready = true;
}