#include <aichat.h>

record Knowledge[] = {
    {
        "aichat",
        {
            "嗯，我在",
            "在这呦~",
            "有什么事喵",
            "呦吼"
        }
    },
    {
        "哈哈",
        {
            "我也很高兴",
            "哈哈哈",
            "哈哈，你笑什么呢？",
            "看起来你很高兴"
        }
    },
    {
        "名字",
        {
            "我的名字是aichat",
            "你可以叫我aichat",
            "aichat随时为您服务",
            "我叫aichat,随时为你服务"
        }
    },
    {
        "宝贝",
        {
            "谢谢你",
            "你嘴真甜",
            "oh baby~",
            "你真好看"
        }
    },
    {
        "hi",
        {
            "你好",
            "今天过的怎么样？",
            "Hello",
            "Hi"
        }
    },
    {
        "你好",
        {
            "你好",
            "今天过的怎么样？",
            "Hello",
            "Hi"
        }
    },
    {
        "hello",
        {
            "你好",
            "今天过的怎么样？",
            "Hello",
            "Hi"
        }
    },
    {
        "真人",
        {
            "这很重要吗？",
            "我并不是真人，我是您的智能聊天助手",
            "如果你觉得我是，我就是",
            "你什么意思"
        }
    },
    {
        "天气",
        {
            "大概率是晴天哦",
            "可能会下雨呢",
            "可能会是多云",
            "等会儿也许会阴天"
        }
    },
    {
        "再见",
        {
            "再见",
            "bye",
            "祝您生活愉快",
            "欢迎再次使用"
        }
    },
    {
        "bye",
        {
            "再见",
            "bye",
            "祝您生活愉快",
            "欢迎再次使用"
        }
    },
    {
        "谢谢",
        {
            "不用谢",
            "感谢您的夸奖",
            "这是我的荣幸",
            "不客气！有什么我可以再帮您的吗？"
        }
    }

};

int KnowledgeSize = sizeof(Knowledge)/sizeof(Knowledge[0]);

QString aichat(QString text)
{
    string input = text.toStdString();
    string response[] = {
        "你说，我在听",
        "无论何时何地，我都会在你身边，很高兴能帮到你",
        "请告诉我更多",
        "和您对话很高兴",
        "这里是aichat，随时为您服务",
        "如果您有其他问题，请随时告诉我。",
        "请告诉我您的问题，我会尽力为您提供帮助。",
        "祝您生活愉快"
    };
    string res_back = "";
    vector<string> res = find_match(input);
    srand(time(nullptr));
    if(res.size() != 0){
        int choose = rand()%MAX_RESP;
        res_back = res[choose];
    }
    else{
        int random = rand()%8;
        res_back = response[random];
        size_t pos=0;
        while((pos=input.find("您",pos))!=string::npos){
            input.replace(pos,3,"你");
            pos+=3;
        }

        pos=0;
        if(input.find("你")!=string::npos or input.find("我")!=string::npos){
            if(!(input.find("你")!=string::npos)){
                pos=0;
                while((pos=input.find("我",pos))!=string::npos){
                    input.replace(pos,3,"我也");
                    pos+=2;
                }
            }
            else{
                while((pos=input.find("你",pos))!=string::npos){
                    input.replace(pos,3,"汝");
                    pos+=2;
                }
                pos=0;
                while((pos=input.find("我",pos))!=string::npos){
                    input.replace(pos,3,"你");
                    pos+=2;
                }
                pos=0;
                while((pos=input.find("汝",pos))!=string::npos){
                    input.replace(pos,3,"我");
                    pos+=2;
                }
            }

            res_back=input;
        }

        if(input.find("吗")!=string::npos){
            if(input.find("吗？")!=string::npos){
                input.replace(input.find("吗？"),6,"。");
            }
            else{
                input.replace(input.find("吗"),3,"。");
            }
            res_back = input;
        }
        else if(input.find("啊")!=string::npos||input.find("呀")!=string::npos||input.find("哇")!=string::npos){
            res_back = input;
        }
        pos=0;
        //
        if(input.find("怎么")!=string::npos||input.find("怎样")!=string::npos){
            if(input.find("怎么样")!=string::npos){
                input.replace(input.find("怎么样"),9,"可以");
                res_back = input;
            }
            else if(input.find("怎样")!=string::npos){
                input.replace(input.find("怎样"),6,"可以");
                res_back = input;
            }
            else{
                input.replace(input.find("怎么"),6,"可以");
                res_back = input;
            }
        }
        if(input.find("会不会")!=string::npos){
            input.replace(input.find("会不会"),9,"不会");
            res_back = input;
        }
        if(input.find("是不是")!=string::npos){
            input.replace(input.find("是不是"),9,"是");
            res_back = input;
        }
        if(input.find("有没有")!=string::npos){
            input.replace(input.find("有没有"),9,"有");
            res_back = input;
        }
        if(input.find("如果")!=string::npos){
            input.replace(input.find("如果"),6,"那么");
            res_back = input;
        }
        if(input.find("什么")!=string::npos){
            input="我不知道"+input;
            res_back = input;
        }
        if(input.find("如何")!=string::npos){
            input="我不知道"+input;
            res_back = input;
        }
    }
    return QString::fromStdString(res_back);
}

vector<string> find_match(string  input) {
    vector<string> result;
    size_t pos=0;
    while((pos=input.find("您",pos))!=string::npos){
        input.replace(pos,3,"你");
        pos+=2;
    }
    for(int i = 0; i < KnowledgeSize;  i++) {
        for(int i = 0; i < KnowledgeSize; i++) {
            if(input.find(string(Knowledge[i].input))!=string::npos) {
                if(input.compare("你好")==0){
                    copy(Knowledge[i].responses, result);

                }
                if(input.find("你好啊")!=string::npos or input.find("你好呀")!=string::npos){
                    copy(Knowledge[i].responses, result);
                }

                copy(Knowledge[i].responses, result);
            }
        }
    }
    return result;
}

void copy(string array[], vector<string> &v) {
    for(int i = 0;  i < MAX_RESP; ++i) {
        v.push_back(array[i]);
    }
}
