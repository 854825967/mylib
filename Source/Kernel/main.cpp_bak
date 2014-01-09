#include "MultiSys.h"
#include "Mysql++/mysql++.h"
#include "../Public/CDumper.h"
#include <vector>
#include <iostream>

class InitDumper {
public:
    InitDumper() {
        CDumper::GetInstance().SetDumpFileName("Kernel_dump");
    }
};

#ifdef _DEBUG
#pragma comment(lib,"D:/Code/mylib/Source/Kernel/Mysql++/mysqlpp_d.lib")
#else
#pragma comment(lib,"D:/Code/mylib/Source/Kernel/Mysql++/mysqlpp.lib")
#endif //_DEBUG

typedef std::vector<mysqlpp::Row> VECTOR_ROW;

s32 main() {
    mysqlpp::Connection connect;
    try {
        connect.connect("accountcenter", "127.0.0.1", "root");
    }
    catch (...) {
    	ECHO_ERROR("连接数据库异常");
        return -1;
    }
    if (connect.connected()) {
        ECHO_TRACE("连接数据库成功");
    } else {
        ECHO_TRACE("连接数据库失败");
        return 0;
    }

    VECTOR_ROW v;
    mysqlpp::Query query = connect.query();
    query.reset();
    query << "call PrGs_GetAccount(1, '12312321', 'afdsafdsafda');";
    try
    {
        mysqlpp::StoreQueryResult res = query.store();
        //std::cout << "Res : " << res[0]["nReturn"] << " | " << res[0]["lAccount"] << std::endl;
    }
    catch (std::ios_base::failure e)
    {
    	std::cout << "错误" << std::endl;
    }


	return 0;
}
