###################################################################
#           Copyright (c) 2021 by TAOS Technologies, Inc.
#                     All rights reserved.
#
#  This file is proprietary and confidential to TAOS Technologies.
#  No part of this file may be reproduced, stored, transmitted,
#  disclosed or used in any form or by any means other than as
#  expressly provided by the written permission from Jianhui Tao
#
###################################################################

# -*- coding: utf-8 -*-

import sys
from util.log import *
from util.cases import *
from util.sql import *


class TDTestCase:
    def init(self, conn, logSql):
        tdLog.debug("start to execute %s" % __file__)
        tdSql.init(conn.cursor(), logSql)
        self._conn = conn 

    def run(self):
        print("running {}".format(__file__))
        tdSql.execute("drop database if exists test")
        tdSql.execute("create database if not exists test precision 'us'")
        tdSql.execute('use test')

        tdSql.execute('create stable ste(ts timestamp, f int) tags(t1 bigint)')

        lines = [
            "st,t1=3i,t2=4,t3=\"t3\" c1=3i,c3=L\"passit\",c2=false,c4=4 1626006833639000000",
            "st,t1=4i,t3=\"t4\",t2=5,t4=5 c1=3i,c3=L\"passitagin\",c2=true,c4=5,c5=5 1626006833640000000",
            "ste,t2=5,t3=L\"ste\" c1=true,c2=4,c3=\"iam\" 1626056811823316532",
            "st,t1=4i,t2=5,t3=\"t4\" c1=3i,c3=L\"passitagain\",c2=true,c4=5 1626006833642000000",
            "ste,t2=5,t3=L\"ste2\" c3=\"iamszhou\",c4=false 1626056811843316532",
            "ste,t2=5,t3=L\"ste2\" c3=\"iamszhou\",c4=false,c5=32b,c6=64s,c7=32w,c8=88.88f 1626056812843316532",
            "st,t1=4i,t3=\"t4\",t2=5,t4=5 c1=3i,c3=L\"passitagin\",c2=true,c4=5,c5=5,c6=7u 1626006933640000000",
            "stf,t1=4i,t3=\"t4\",t2=5,t4=5 c1=3i,c3=L\"passitagin\",c2=true,c4=5,c5=5,c6=7u 1626006933640000000",
            "stf,t1=4i,t3=\"t4\",t2=5,t4=5 c1=3i,c3=L\"passitagin_stf\",c2=false,c5=5,c6=7u 1626006933641a"]

        code = self._conn.insertLines(lines)
        print("insertLines result {}".format(code))

        lines2 = [
                "stg,t1=3i,t2=4,t3=\"t3\" c1=3i,c3=L\"passit\",c2=false,c4=4 1626006833639000000",
                "stg,t1=4i,t3=\"t4\",t2=5,t4=5 c1=3i,c3=L\"passitagin\",c2=true,c4=5,c5=5 1626006833640000000"]
        
        code = self._conn.insertLines([ lines2[0] ])
        print("insertLines result {}".format(code))

        self._conn.insertLines([ lines2[1] ])
        print("insertLines result {}".format(code))

        tdSql.query("select * from st");
        tdSql.checkRows(4)

        tdSql.query("select * from ste");
        tdSql.checkRows(3)

        tdSql.query("select * from stf");
        tdSql.checkRows(2)

        tdSql.query("select * from stg");
        tdSql.checkRows(2)

        tdSql.query("show tables");
        tdSql.checkRows(8)

        tdSql.query("describe stf");
        tdSql.checkData(3,2, 14)

    def stop(self):
        tdSql.close()
        tdLog.success("%s successfully executed" % __file__)


tdCases.addWindows(__file__, TDTestCase())
tdCases.addLinux(__file__, TDTestCase())
