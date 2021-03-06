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
	tcpSocket = new QTcpSocket();
    if (!tcpSocket->setSocketDescriptor(socketDescriptor)) {
        emit error(tcpSocket->error());
        qDebug() << "socket error!";
		return;
    }
	connect(tcpSocket, &QTcpSocket::readyRead, this, &UserThread::readData);
	connect(this, &UserThread::readPackage, this, &UserThread::handlePackage);
    connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
	in.setDevice(tcpSocket);
    in.setVersion(QDataStream::Qt_5_0);
}

void UserThread::run() {
	qDebug() << "enter run"; 
    // while(!ready) ; //busy waiting for data ready, not a good idea
    //                 //maybe mutex or semaphor is better
	//qDebug() << "wait disconnect";
	//tcpSocket->waitForDisconnected(-1);
	exec();
	//qDebug() << "disconnecting";
    //exec();
}

void UserThread::readData() {
   	qDebug() << "reading"; 
    //in.startTransaction();
    static int flag = 0;
	static std::string raw;
	int total = tcpSocket->bytesAvailable();
	//qDebug() << total;
	while(total--) {
		in.readRawData(data, 1);
    	if(data[0] == '#')
			flag ++;
		else
			flag = 0;
		raw.append(std::string(data,1));
		if(flag == 10) {
    		break;
		}
	}
    
	if(flag == 10) { //a complete package
		//qDebug() << "Complete package";	
		flag = 0;
		for(int i=0; i<10; i++)
			raw.pop_back(); //pop 10#
    	dp.ParseFromString(raw);
   		qDebug() << "new package: " << raw.length(); 
		raw.clear();
    	emit readPackage();
	}
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
			//qDebug() << "request login";
            const ::google::protobuf::Any& any = dp.body();
			qDebug() << any.Is<UserInfo>();
            if(any.Is<UserInfo>()) { // must be true 
                UserInfo userInfo;
                any.UnpackTo(&userInfo);
                std::string username = userInfo.username();
                std::string password = userInfo.password();
                Model *m = database.getModelByName(user, username.c_str());
				qDebug() << username.c_str() << password.c_str();
				if(!m) qDebug() << "model is null!";
                if(m && dynamic_cast<UserModel*>(m)->getPass() == password) {
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
				//return;
            break;
        }

        case Header::REGISTER : {
            const ::google::protobuf::Any& any = dp.body();
            if(any.Is<UserInfo>()) { // must be true 
                UserInfo userInfo;
                any.UnpackTo(&userInfo);
                std::string username = userInfo.username();
                std::string password = userInfo.password();
				qDebug() << "Register" << username.c_str() << password.c_str();
                Model *m = new UserModel(username, password);
                if(database.addModel(m)) { //OK, this username is not used
                    delete m;
					m = database.getModelByName(user,username.c_str());
					userId = m->getId();
					qDebug() << "UserId :" << userId;
                    UserThread::onlineUsers.push_back(userId);
                    UserThread::sockets.insert(userId, socketDescriptor);
					Header *header = ret.mutable_header();
                    header->set_type(Header::REPONSE);
                    header->set_resource(Header::REGISTER);
                    header->set_status(Header::OK);
				} 
                else { // the username is used                  
                    Header *header = ret.mutable_header();
                    header->set_type(Header::REPONSE);
                    header->set_resource(Header::REGISTER);
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
				qDebug() << "Search Music";
                MusicInfo musicInfo;
                any.UnpackTo(&musicInfo);
                std::string name = musicInfo.name();
                std::vector<int> musicIdVector0 = database.getMusicsByName(name.c_str());
                //std::vector<int> musicIdVector1 = database.getMusicsBySinger(name.c_str());
                for(std::vector<int>::iterator it = musicIdVector0.begin(); it != musicIdVector0.end(); it ++) {
                    Model *m = database.getModelById(music, *it);
                    qDebug() << "Music " << *it;
					MusicInfo *mi = ml.add_musicinfo();
                    qDebug() << (dynamic_cast<MusicModel*>(m)->getName()).c_str();
                    mi->set_name(dynamic_cast<MusicModel*>(m)->getName());
                    //mi->set_singer(dynamic_cast<MusicModel*>(m)->getSinger());
                    delete m;
                }
                // for(std::vector<int>::iterator it = musicIdVector1.begin(); it != musicIdVector1.end(); it ++) {
                //     Model *m = database.getModelById(music, *it);
                //     MusicInfo *mi = ml.add_musicinfo();
                //     mi->set_name(dynamic_cast<MusicModel*>(m)->getName());
                //     mi->set_singer(dynamic_cast<MusicModel*>(m)->getSinger());
                //     delete m;
                // }
                Header *header = ret.mutable_header();
                header->set_type(Header::REPONSE);
                header->set_resource(Header::SEARCH_MUSIC);
                header->set_status(Header::OK);
                ret.mutable_body()->PackFrom(ml);
				qDebug() << "Search end";
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
				qDebug() << "Search user" << username.c_str();
                Model *m = database.getModelByName(user, username.c_str());
                if(m) { //the user exist
					qDebug() << "User " << m->getId() << "exsit";
                    Header *header = ret.mutable_header();
                    header->set_type(Header::REPONSE);
                    header->set_resource(Header::SEARCH_USER);
                    header->set_status(Header::OK);
                    ui.set_username(username);
                    if(UserThread::onlineUsers.contains(m->getId())){
						qDebug() << "user "  << m->getId() << " online";
                        ui.set_status(UserInfo::ONLINE);
					}
                    else {
						qDebug() << "user " << m->getId() << " offline";
                        ui.set_status(UserInfo::OFFLINE);
					}
					ret.mutable_body()->PackFrom(ui);
                }
                else { // not exist
					qDebug() << "User not exsit";
                    Header *header = ret.mutable_header();
                    header->set_type(Header::REPONSE);
                    header->set_resource(Header::SEARCH_USER);
                    header->set_status(Header::NOTFOUND);
                }
				delete m;
            }
            else 
                assert(0);           
            break;
        }

        case Header::COMMENT : {
            const ::google::protobuf::Any& any = dp.body();
            if(any.Is<CommentInfo>()) { // must be true 
				qDebug() << "comment";
                CommentInfo commentInfo;
                any.UnpackTo(&commentInfo);
                std::string username = commentInfo.username();
                std::string musicname = commentInfo.musicname();
                Model *um = database.getModelByName(user, username.c_str());
                Model *mm = database.getModelByName(music, musicname.c_str());
                Model *m = new CommentModel(commentInfo.content(), um->getId(), mm->getId(), 0, "");
              	qDebug() << "User" << um->getId() << " comment in music " << mm->getId() << " with " << commentInfo.content().c_str();
				qDebug() << "Add comment " << database.addModel(m);
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
				qDebug() << "Get comments";
                MusicInfo musicInfo;
                any.UnpackTo(&musicInfo);
                Model *m = database.getModelByName(music, musicInfo.name().c_str());
				assert(m);		
                std::vector<int> commentIdVector = database.getCommentsByMusicId(m->getId());
                delete m;
                for(std::vector<int>::iterator it = commentIdVector.begin(); it!=commentIdVector.end(); it++) {
                    m = database.getModelById(comment, *it);
					qDebug() << "Fetch comment" << *it << dynamic_cast<CommentModel*>(m)->getContent().c_str() << "by" << dynamic_cast<CommentModel*>(m)->getUserId();
                    CommentInfo *ci = cl.add_commentinfo();
                    //ci->set_id(m->getId());
                    ci->set_content(dynamic_cast<CommentModel*>(m)->getContent());
                    Model *um = database.getModelById(user, dynamic_cast<CommentModel*>(m)->getUserId());
                    ci->set_username(dynamic_cast<UserModel*>(um)->getName());
                    delete um;
                    //ci->set_date(dynamic_cast<CommentModel*>(m)->getDate());
                    //ci->set_thumb(dynamic_cast<CommentModel*>(m)->getThumb());
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
                std::string musicpath = "./music/" + musicname + ".wav";
				std::string lyricpath = "./music/" + musicname + ".lrc";
                std::ifstream f1(musicpath.c_str(), std::ios::in|std::ios::binary);
				std::ifstream f2(lyricpath.c_str(), std::ios::in|std::ios::binary);
                std::string data((std::istreambuf_iterator<char>(f1)),  std::istreambuf_iterator<char>());  //read music data into string
                std::string lyric((std::istreambuf_iterator<char>(f2)),  std::istreambuf_iterator<char>());  //read lyric data into string
                md.set_data(data);
				md.set_lyric(lyric);
				qDebug() << musicpath.c_str() << data.length() << lyricpath.c_str() << lyric.length();
                f1.close();
				f2.close();
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
            if(isLogin()) {
            	UserThread::onlineUsers.removeOne(userId);
                QMap<int, qintptr>::iterator it = UserThread::sockets.find(userId);
				UserThread::sockets.erase(it);
                userId = -1;
            	Header *header = ret.mutable_header();
            	header->set_type(Header::REPONSE);
            	header->set_resource(Header::LOGOUT);
            	header->set_status(Header::OK);
			}
			else {
				Header *header = ret.mutable_header();
            	header->set_type(Header::REPONSE);
            	header->set_resource(Header::LOGOUT);
            	header->set_status(Header::ERROR);
			}
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
				qDebug() << "chat";
                Text txt;
                any.UnpackTo(&txt);
                std::string dst = txt.dst();
                qDebug() << "Chat with " << dst.c_str();
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
                }
                else {
                    qintptr tmp = it.value();
                    std::string src = txt.src();
                    std::string text = txt.text();
                    emit sendMsg(tmp, src, text);
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

        case Header::UPLOAD : {
            qDebug() << "Request upload music";
            const ::google::protobuf::Any& any = dp.body();
            if(any.Is<MusicData>()) { // must be true 
                MusicData md;
                any.UnpackTo(&md);
                std::string musicname = md.name();
				qDebug() << musicname.c_str();
                Model *m = database.getModelByName(music, musicname.c_str());
                if(!m) { //music not exist
                    qDebug() << "uploading!";
                    std::string data = md.data();
                    std::string lyric = md.lyric();
                    std::string musicpath = "./music/" + musicname + ".wav";
				    std::string lyricpath = "./music/" + musicname + ".lrc";
                    std::ofstream f1(musicpath.c_str(), std::ios::out|std::ios::binary);
				    std::ofstream f2(lyricpath.c_str(), std::ios::out|std::ios::binary);
                    f1.write(data.c_str(), data.length());
                    f2.write(lyric.c_str(), lyric.length());
                    f1.close();
                    f2.close();
					//add infomation into database
					m = new MusicModel(musicname, "");
					database.addModel(m);
					delete m;
                    Header *header = ret.mutable_header();
                    header->set_type(Header::REPONSE);
                    header->set_resource(Header::UPLOAD);
                    header->set_status(Header::OK);
                    qDebug() << "upload finish!";
                }
                else {
                    Header *header = ret.mutable_header();
                    header->set_type(Header::REPONSE);
                    header->set_resource(Header::UPLOAD);
                    header->set_status(Header::ERROR);
                	delete m; //free memory
				}
            }
            else 
                assert(0);           
            break;
        }

        default : assert(0);
    }
    std::string raw;
    ret.SerializeToString(&raw);
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_0);
    out.writeRawData(raw.c_str(), raw.length());
    qDebug() << tcpSocket->write(block);
	tcpSocket->write("##########");
	tcpSocket->flush();
	qDebug() << "Reponse finish";
}

bool UserThread::isLogin() {
    return !(userId == -1);
}

void UserThread::onDisconnected() {
	qDebug() << "disconnected";
    database.close();
    UserThread::onlineUsers.removeOne(userId);
    UserThread::sockets.remove(userId);
    tcpSocket->close();
    quit();
}

void UserThread::sendMessage(std::string src, std::string text) {
    qDebug() << "message sent by " << src.c_str(); 
    Text send;
    Datapackage sendDp;
    send.set_src(src);
    send.set_text(text);
    Header *header = sendDp.mutable_header();
    header->set_type(Header::REQUEST);
    header->set_resource(Header::CHAT);
    header->set_status(Header::OK);
    sendDp.mutable_body()->PackFrom(send);
    std::string data; 
    sendDp.SerializeToString(&data);
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_0);
    out.writeRawData(data.c_str(), data.length());
    tcpSocket->write(block);
    tcpSocket->write("##########");
    tcpSocket->flush();
}
