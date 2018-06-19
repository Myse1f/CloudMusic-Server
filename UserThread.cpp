#include "UserThread.h"
#include <QtCore/QDataStream>
#include <QtCore/QByteArray>
#include <string>
#include <vector>
#include <fstream>
#include <assert.h>
#include "UserModel.h"
#include "MusicModel.h"
#include "CommentModel.h"
#include "RoleModel.h"
#include "AuthorityModel.h"
#include "QtDebug"

QMap<int,qintptr> UserThread::sockets;
QVector<int> UserThread::onlineUsers;

UserThread::UserThread(int socketDescriptor, QObject *parent): QThread(parent), socketDescriptor(socketDescriptor) {
    userId = -1;
	qDebug() << "new Thread";
    if (!tcpSocket.setSocketDescriptor(socketDescriptor)) {
        emit error(tcpSocket.error());
        qDebug() << "socket error!";
		return;
    }
	connect(&tcpSocket, &QIODevice::readyRead, this, &UserThread::readData);
	connect(this, &UserThread::readPackage, this, &UserThread::handlePackage);
    connect(&tcpSocket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));

}

void UserThread::run() {
	qDebug() << "enter run";
    
    in.setDevice(&tcpSocket);
    in.setVersion(QDataStream::Qt_5_0);

    // while(!ready) ; //busy waiting for data ready, not a good idea
    //                 //maybe mutex or semaphor is better
	//qDebug() << "wait disconnect";
	//tcpSocket.waitForDisconnected(-1);
	//qDebug() << "disconnected";
    exec();
}

void UserThread::readData() {
    in.startTransaction();
    
    in >> data;
    if(!in.commitTransaction())
        return;
    // if(nextData == currentData)
    //     return;

    std::string raw = data.toStdString();
    dp.ParseFromString(raw);
    
    emit readPackage();
}

// void UserThread::terminateThread() {
    
// }

void UserThread::handlePackage() {
    // parse header
    Header h = dp.header();
    Datapackage ret;
    // assert(h.type() == Header::REQUEST);
    switch(h.resource()) {
        case Header::LOGIN : {
            const ::google::protobuf::Any& any = dp.body();
            if(any.Is<UserInfo>()) { // must be true 
                UserInfo userInfo;
                any.UnpackTo(&userInfo);
                std::string username = userInfo.username();
                std::string password = userInfo.password();
                Model *m = database.getModelByName(user, username.c_str());
                if(!m && dynamic_cast<UserModel*>(m)->getPass() == password) {
                    userId = m->getId();
                    UserThread::onlineUsers.push_back(userId);
                    UserThread::sockets.insert(userId, socketDescriptor);
                    // reponse with OK                   
                    Header *header = ret.mutable_header();
                    header->set_type(Header::REPONSE);
                    header->set_resource(Header::LOGIN);
                    header->set_status(Header::OK);
                }
                else {
                    // reponse with ERROR                   
                    Header *header = ret.mutable_header();
                    header->set_type(Header::REPONSE);
                    header->set_resource(Header::LOGIN);
                    header->set_status(Header::ERROR);
                }
                delete m; //free memory
            }
            else 
                assert(0);           
            break;
        }

        case Header::REGISTER : {
            const ::google::protobuf::Any& any = dp.body();
            if(any.Is<UserInfo>()) { // must be true 
                UserInfo userInfo;
                any.UnpackTo(&userInfo);
                std::string username = userInfo.username();
                std::string password = userInfo.password();
                Model *m = new UserModel(username, password);
                if(database.addModel(m)) { //OK, this username is not used
                    Header *header = ret.mutable_header();
                    header->set_type(Header::REPONSE);
                    header->set_resource(Header::LOGIN);
                    header->set_status(Header::OK);
                } 
                else { // the username is used                  
                    Header *header = ret.mutable_header();
                    header->set_type(Header::REPONSE);
                    header->set_resource(Header::LOGIN);
                    header->set_status(Header::ERROR);
                }
                delete m;
            }
            else 
                assert(0);           
            break;
        }

        case Header::SEARCH_MUSIC : {
            const ::google::protobuf::Any& any = dp.body();
            MusicList ml;
            if(any.Is<MusicInfo>()) { // must be true 
                MusicInfo musicInfo;
                any.UnpackTo(&musicInfo);
                std::string name = musicInfo.name();
                std::vector<int> musicIdVector0 = database.getMusicsByName(name.c_str());
                std::vector<int> musicIdVector1 = database.getMusicsBySinger(name.c_str());
                for(std::vector<int>::iterator it = musicIdVector0.begin(); it != musicIdVector0.end(); it ++) {
                    Model *m = database.getModelById(music, *it);
                    MusicInfo *mi = ml.add_musicinfo();
                    mi->set_name(dynamic_cast<MusicModel*>(m)->getName());
                    mi->set_singer(dynamic_cast<MusicModel*>(m)->getSinger());
                    delete m;
                }
                for(std::vector<int>::iterator it = musicIdVector1.begin(); it != musicIdVector1.end(); it ++) {
                    Model *m = database.getModelById(music, *it);
                    MusicInfo *mi = ml.add_musicinfo();
                    mi->set_name(dynamic_cast<MusicModel*>(m)->getName());
                    mi->set_singer(dynamic_cast<MusicModel*>(m)->getSinger());
                    delete m;
                }
                Header *header = ret.mutable_header();
                header->set_type(Header::REPONSE);
                header->set_resource(Header::SEARCH_MUSIC);
                header->set_status(Header::OK);
                ret.mutable_body()->PackFrom(ml);
            }
            else 
                assert(0);           
            break;
        }

        case Header::SEARCH_USER : {
            const ::google::protobuf::Any& any = dp.body();
            UserInfo ui;
            if(any.Is<UserInfo>()) { // must be true 
                UserInfo userInfo;
                any.UnpackTo(&userInfo);
                std::string username = userInfo.username();
                Model *m = database.getModelByName(user, username.c_str());
                if(!m) { //the user exist
                    Header *header = ret.mutable_header();
                    header->set_type(Header::REPONSE);
                    header->set_resource(Header::SEARCH_USER);
                    header->set_status(Header::OK);
                    ui.set_username(username);
                    if(UserThread::onlineUsers.contains(m->getId()))
                        ui.set_status(UserInfo::ONLINE);
                    else
                        ui.set_status(UserInfo::OFFLINE);
                }
                else { // not exist
                    Header *header = ret.mutable_header();
                    header->set_type(Header::REPONSE);
                    header->set_resource(Header::SEARCH_USER);
                    header->set_status(Header::NOTFOUND);
                }
            }
            else 
                assert(0);           
            break;
        }

        case Header::COMMENT : {
            const ::google::protobuf::Any& any = dp.body();
            if(any.Is<CommentInfo>()) { // must be true 
                CommentInfo commentInfo;
                any.UnpackTo(&commentInfo);
                std::string username = commentInfo.username();
                std::string musicname = commentInfo.musicname();
                Model *um = database.getModelByName(user, username.c_str());
                Model *mm = database.getModelByName(music, musicname.c_str());
                Model *m = new CommentModel(commentInfo.content(), um->getId(), mm->getId(), 0, commentInfo.date());
                database.addModel(m);
                delete um; delete mm; delete m;
                Header *header = ret.mutable_header();
                header->set_type(Header::REPONSE);
                header->set_resource(Header::COMMENT);
                header->set_status(Header::OK);
            }
            else 
                assert(0);           
            break;
        }

		case Header::LIKE : {
            const ::google::protobuf::Any& any = dp.body();
            if(any.Is<CommentInfo>()) { // must be true 
                CommentInfo commentInfo;
                any.UnpackTo(&commentInfo);
                int id = commentInfo.id();
                Model *m = database.getModelById(comment, id);
                dynamic_cast<CommentModel*>(m)->like();
                database.modifyModel(m);
                delete m;
                Header *header = ret.mutable_header();
                header->set_type(Header::REPONSE);
                header->set_resource(Header::LIKE);
                header->set_status(Header::OK);
            }
            else 
                assert(0);           
            break;
        }

		case Header::GET_COMMENTS : {
            const ::google::protobuf::Any& any = dp.body();
            CommentList cl;
            if(any.Is<MusicInfo>()) { // must be true 
                MusicInfo musicInfo;
                any.UnpackTo(&musicInfo);
                Model *m = database.getModelByName(music, musicInfo.name().c_str());
                std::vector<int> commentIdVector = database.getCommentsByMusicId(m->getId());
                delete m;
                for(std::vector<int>::iterator it = commentIdVector.begin(); it!=commentIdVector.end(); it++) {
                    m = database.getModelById(comment, *it);
                    CommentInfo *ci = cl.add_commentinfo();
                    ci->set_id(m->getId());
                    ci->set_content(dynamic_cast<CommentModel*>(m)->getContent());
                    Model *um = database.getModelById(user, dynamic_cast<CommentModel*>(m)->getUserId());
                    ci->set_username(dynamic_cast<UserModel*>(m)->getName());
                    delete um;
                    ci->set_date(dynamic_cast<CommentModel*>(m)->getDate());
                    ci->set_thumb(dynamic_cast<CommentModel*>(m)->getThumb());
                    delete m;
                }
                Header *header = ret.mutable_header();
                header->set_type(Header::REPONSE);
                header->set_resource(Header::GET_COMMENTS);
                header->set_status(Header::OK);
                ret.mutable_body()->PackFrom(cl);
            }
            else 
                assert(0);           
            break;
        }

		case Header::GET_MUSICS : { //get user's music
            const ::google::protobuf::Any& any = dp.body();
            MusicList ml;
            if(any.Is<UserInfo>()) { // must be true 
                UserInfo userInfo;
                any.UnpackTo(&userInfo);
                std::string username = userInfo.username();
                Model *m = database.getModelByName(user, username.c_str());
                std::vector<int> musicIdVector = database.getMusicsByUserId(m->getId());
                delete m;
                for(std::vector<int>::iterator it = musicIdVector.begin(); it!=musicIdVector.end(); it++) {
                    m = database.getModelById(music, *it);
                    MusicInfo *mi = ml.add_musicinfo();
                    mi->set_name(dynamic_cast<MusicModel*>(m)->getName());
                    mi->set_singer(dynamic_cast<MusicModel*>(m)->getSinger());
                    delete m;
                }
                Header *header = ret.mutable_header();
                header->set_type(Header::REPONSE);
                header->set_resource(Header::GET_MUSICS);
                header->set_status(Header::OK);
                ret.mutable_body()->PackFrom(ml);
            }
            else 
                assert(0);           
            break;
        }
        
		case Header::GET_MUSICFILE : {
            const ::google::protobuf::Any& any = dp.body();
            MusicData md;
            if(any.Is<MusicInfo>()) { // must be true 
                MusicInfo musicInfo;
                any.UnpackTo(&musicInfo);
                std::string musicname = musicInfo.name();
                std::string path = "./music/" + musicname;
                std::ifstream f(path.c_str(), std::ios::in|std::ios::binary);  
                std::string data((std::istreambuf_iterator<char>(f)),  std::istreambuf_iterator<char>());  //read data into string
                md.set_data(data);
                f.close();
                Header *header = ret.mutable_header();
                header->set_type(Header::REPONSE);
                header->set_resource(Header::GET_MUSICFILE);
                header->set_status(Header::OK);
                ret.mutable_body()->PackFrom(md);
            }
            else 
                assert(0);           
            break;
        }

		case Header::GET_USERS : {
            const ::google::protobuf::Any& any = dp.body();
            UserList ul;
            if(any.Is<UserInfo>()) { // must be true 
                UserInfo userInfo;
                any.UnpackTo(&userInfo);
                std::string username = userInfo.username();
                Model *m = database.getModelByName(user, username.c_str());
                std::vector<int> userIdVector = database.getFolloweesByUserId(m->getId());
                delete m;
                for(std::vector<int>::iterator it = userIdVector.begin(); it!=userIdVector.end(); it++) {
                    m = database.getModelById(user, *it);
                    UserInfo *ui = ul.add_userinfo();
                    ui->set_username(dynamic_cast<UserModel*>(m)->getName());
                    delete m;
                }
                Header *header = ret.mutable_header();
                header->set_type(Header::REPONSE);
                header->set_resource(Header::GET_USERS);
                header->set_status(Header::OK);
                ret.mutable_body()->PackFrom(ul);
            }
            else 
                assert(0);           
            break;
        }

        case Header::LOGOUT : {
            if(isLogin())
                userId = -1;
            UserThread::onlineUsers.removeOne(userId);
            Header *header = ret.mutable_header();
            header->set_type(Header::REPONSE);
            header->set_resource(Header::LOGOUT);
            header->set_status(Header::OK);
            break;
        }

        case Header::FOLLOW : {
            const ::google::protobuf::Any& any = dp.body();
            if(any.Is<UserInfo>()) { // must be true 
                UserInfo userInfo;
                any.UnpackTo(&userInfo);
                std::string username = userInfo.username();
                Model *m = database.getModelByName(user, username.c_str());
                database.addFollow(userId, m->getId());
                Header *header = ret.mutable_header();
                header->set_type(Header::REPONSE);
                header->set_resource(Header::FOLLOW);
                header->set_status(Header::OK);
            }
            else 
                assert(0);           
            break;
        }

        case Header::CHAT : {
            const ::google::protobuf::Any& any = dp.body();
            if(any.Is<Text>()) {
                Text txt;
                any.UnpackTo(&txt);
                std::string dst = txt.dst();
                Model *m = database.getModelByName(user, dst.c_str());
                assert(m);
                //std::string text = txt.text();
                QMap<int, qintptr>::iterator it = UserThread::sockets.find(m->getId());
                if(it == sockets.end()) {
                    qDebug() << "User is offline";
                    Header *header = ret.mutable_header();
                    header->set_type(Header::REPONSE);
                    header->set_resource(Header::CHAT);
                    header->set_status(Header::ERROR);
                    return;
                }
                else {
                    qintptr tmp = it.value();
                    Datapackage anotherDp;
                    QTcpSocket tcp;
                    Text send;
                    if(!tcp.setSocketDescriptor(tmp)) {
                        emit error(tcpSocket.error());
                        qDebug() << "socket error!";
                        return;
                    }
                    send.set_src(txt.src());
                    send.set_dst(txt.dst());
                    send.set_text(txt.text());
                    Header *header1 = anotherDp.mutable_header();
                    header1->set_type(Header::REQUEST);
                    header1->set_resource(Header::CHAT);
                    header1->set_status(Header::OK);
                    anotherDp.mutable_body()->PackFrom(send);
                    std::string data1; 
                    anotherDp.SerializeToString(&data1);
                    QByteArray block;
                    QDataStream out1(&block, QIODevice::WriteOnly);
                    out1.setVersion(QDataStream::Qt_5_0);
                    out1 << data1.c_str();
                    tcp.write(block);
                    Header *header = ret.mutable_header();
                    header->set_type(Header::REPONSE);
                    header->set_resource(Header::CHAT);
                    header->set_status(Header::OK);
                }
            }
            else
                assert(0);
            break;
        }

        default : assert(0);
    }
    std::string data; 
    ret.SerializeToString(&data);
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_0);
    out << data.c_str();
    tcpSocket.write(block);
}

bool UserThread::isLogin() {
    return !(userId == -1);
}

void UserThread::onDisconnected() {
	qDebug() << "disconnected";
    database.close();
    UserThread::onlineUsers.removeOne(userId);
    UserThread::sockets.remove(userId);
    tcpSocket.close();
    quit();
}
