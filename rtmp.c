 typedef struct RTMP_LNK
  {
    AVal hostname; /*主机名*/
    AVal sockshost;

    AVal playpath0;	/* parsed from URL */
    AVal playpath;	/* passed in explicitly */
    AVal tcUrl;     //目标流地址
    AVal swfUrl;   //没必要传输
    AVal pageUrl;  //没必要传输
    AVal app;       //要在服务器中哪个应用程序找这个视频源
    AVal flashVer;
    AVal subscribepath;

    AVal auth;         //用于安全验证
    AVal usherToken;
    AVal token;
    AVal pubUser;
    AVal pubPasswd;

    AMFObject extras;
    int edepth;

    int seekTime;       //会话参数
    int stopTime;

#define RTMP_LF_AUTH	0x0001	/* using auth param */
#define RTMP_LF_LIVE	0x0002	/* stream is live */
#define RTMP_LF_SWFV	0x0004	/* do SWF verification */
#define RTMP_LF_PLST	0x0008	/* send playlist before play */
#define RTMP_LF_BUFX	0x0010	/* toggle stream on BufferEmpty msg */
#define RTMP_LF_FTCU	0x0020	/* free tcUrl on close */
    int lFlags;

    int swfAge;

    int protocol;       /*rtmp 协议*/
    int timeout;		/* connection timeout in seconds */

#define RTMP_PUB_NAME   0x0001  /* send login to server */
#define RTMP_PUB_RESP   0x0002  /* send salted password hash */
#define RTMP_PUB_ALLOC  0x0004  /* allocated data for new tcUrl & app */
#define RTMP_PUB_CLEAN  0x0008  /* need to free allocated data for newer tcUrl & app at exit */
#define RTMP_PUB_CLATE  0x0010  /* late clean tcUrl & app at exit */
    int pFlags;

    unsigned short socksport;
    unsigned short port;

#ifdef CRYPTO
#define RTMP_SWF_HASHLEN	32
    void *dh;			/* for encryption */
    void *rc4keyIn;
    void *rc4keyOut;

    uint32_t SWFSize;
    uint8_t SWFHash[RTMP_SWF_HASHLEN];
    char SWFVerificationResponse[RTMP_SWF_HASHLEN+10];
#endif
  } RTMP_LNK;


typedef struct RTMP
  {
    int m_inChunkSize;
    int m_outChunkSize;     //chunk size，大于chunksize的块都会被分割成chunksize大小
    int m_nBWCheckCounter;
    int m_nBytesIn;
    int m_nBytesInSent;
    int m_nBufferMS;
    int m_stream_id;        /* returned in _result from createStream */
    int m_mediaChannel;
    uint32_t m_mediaStamp;
    uint32_t m_pauseStamp;
    int m_pausing;
    int m_nServerBW;        //window ack size
    int m_nClientBW;        //set peer band
    uint8_t m_nClientBW2;
    uint8_t m_bPlaying;
    uint8_t m_bSendEncoding;
    uint8_t m_bSendCounter;

    int m_numInvokes;   //发送了多少个调用函数给服务器。
    int m_numCalls;
    RTMP_METHOD *m_methodCalls; /* remote method calls queue */

    int m_channelsAllocatedIn;  //当前用来保存csid的输入流的指针有多少个
    int m_channelsAllocatedOut; //当前用来保存csid的输出流的指针有多少个
    RTMPPacket **m_vecChannelsIn;
    RTMPPacket **m_vecChannelsOut;  //是指向存储不同CSID的RTMPPacket指针的指针，每个RTMPPacket指针指向不同chunk流下最新的发送的chunk块，用于压缩每个chunk块的大小，chunk块可以依赖上一个chunk块
    int *m_channelTimestamp;    /* abs timestamp of last packet */

    double m_fAudioCodecs;  /* audioCodecs for the connect packet */
    double m_fVideoCodecs;  /* videoCodecs for the connect packet */
    double m_fEncoding;     /* AMF0 or AMF3 */

    double m_fDuration;     /* duration of stream in seconds */

    int m_msgCounter;       /* RTMPT stuff */
    int m_polling;
    int m_resplen;
    int m_unackd;
    AVal m_clientID;

    RTMP_READ m_read;
    RTMPPacket m_write;
    RTMPSockBuf m_sb;       //C Socket对象，用来发送和读取数据
    RTMP_LNK Link;          //rtmp url的解析大部分就是为了构造此对象，它包括协议、hostname、port、 Connection命令的参数（在rtmp握手成功后第一个发送的命令），还有其他 = 的option参数（包括app名字，flash版本号，编解码器）
    //lake
    char ipaddr[16];
  } RTMP;



  typedef struct RTMPChunk
  {
    int c_headerSize;
    int c_chunkSize;
    char *c_chunk;
    char c_header[RTMP_MAX_HEADER_SIZE];
  } RTMPChunk;

    //Chunk块信息
      typedef struct RTMPPacket
      {  
        uint8_t m_headerType;//ChunkMsgHeader的类型（4种）  
        uint8_t m_packetType;//Message type ID（1-7协议控制；8，9音视频；10以后为AMF编码消息）  
        uint8_t m_hasAbsTimestamp;  /* Timestamp 是绝对值还是相对值? */  
        int m_nChannel;         //块流ID  
        uint32_t m_nTimeStamp;  // Timestamp  
        int32_t m_nInfoField2;  /* last 4 bytes in a long header,消息流ID */  
        uint32_t m_nBodySize;   //消息长度  
        uint32_t m_nBytesRead;  
        RTMPChunk *m_chunk;  
        char *m_body;  
      } RTMPPacket;  


typedef struct AMFObject {
    int o_num;
    struct AMFObjectProperty *o_props;
} AMFObject;

typedef struct AMFObjectProperty {
    AVal p_name;
    AMFDataType p_type;
    union {
        double p_number;
        AVal p_aval;
        AMFObject p_object;
    } p_vu;
    int16_t p_UTCoffset;
} AMFObjectProperty;



typedef struct RTMPSockBuf {
    int sb_socket;      //套接字描述符
    int sb_size;        /* number of unprocessed bytes in buffer */
    char *sb_start;        /* pointer into sb_pBuffer of next byte to process */
    char sb_buf[RTMP_BUFFER_CACHE_SIZE];    /* data read from socket */
    int sb_timedout;    
    void *sb_ssl;
} RTMPSockBuf;

/* state for read() wrapper */
typedef struct RTMP_READ {
    char *buf;
    char *bufpos;
    unsigned int buflen;
    uint32_t timestamp;
    uint8_t dataType;
    uint8_t flags;
#define RTMP_READ_HEADER    0x01
#define RTMP_READ_RESUME    0x02
#define RTMP_READ_NO_IGNORE    0x04
#define RTMP_READ_GOTKF        0x08
#define RTMP_READ_GOTFLVK    0x10
#define RTMP_READ_SEEKING    0x20
    int8_t status;
#define RTMP_READ_COMPLETE    -3
#define RTMP_READ_ERROR    -2
#define RTMP_READ_EOF    -1
#define RTMP_READ_IGNORE    0

    /* if bResume == TRUE */
    uint8_t initialFrameType;
    uint32_t nResumeTS;
    char *metaHeader;
    char *initialFrame;
    uint32_t nMetaHeaderSize;
    uint32_t nInitialFrameSize;
    uint32_t nIgnoredFrameCounter;
    uint32_t nIgnoredFlvFrameCounter;
} RTMP_READ;

connect 命令消息
packet.m_nChannel = 0x03;    /* control channel (invoke) csid */
    packet.m_headerType = RTMP_PACKET_SIZE_LARGE;  //header type 0类型，messageHeader占11字节
    packet.m_packetType = RTMP_PACKET_TYPE_INVOKE;  //commandMessage  信息类型
    packet.m_nTimeStamp = 0;             //时间戳
    packet.m_nInfoField2 = 0;           //信息流id msid
    packet.m_hasAbsTimestamp = 0;       //是否是绝对时间
    packet.m_body = pbuf + RTMP_MAX_HEADER_SIZE;   //最大占18字节的头部 （type = 2 basicHeader(3byte) + messageHeader(11) + extendsStamp(4)）
