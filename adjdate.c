#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <memory.h>
#include <string.h>

#define GAP_DAYS  7168

#define GAP_SECS  (GAP_DAYS*24*3600)

char g_years[][6] = {
    "1999-", "2000-", "2001-"
};

FILE	*infile, *outfile;  /* input & output files */

#define LINE_MAX 8096

char line[LINE_MAX+1];

#ifndef true
#define true 1
#define false 0
#endif

#define DBG //printf
#define VERBOSE printf

#if 0
struct tm {
    int tm_sec;  /*�룬������Χ0-59�� ��������61*/
    int tm_min;  /*���ӣ�0-59*/
    int tm_hour; /*Сʱ�� 0-23*/
    int tm_mday; /*�գ���һ�����еĵڼ��죬1-31*/
    int tm_mon;  /*�£� ��һ������0-11*/  1+p->tm_mon;
    int tm_year;  /*�꣬ ��1900�����Ѿ�������*/  1900�� p->tm_year;
    int tm_wday; /*���ڣ�һ���еĵڼ��죬 ������������0-6*/
    int tm_yday; /*�ӽ���1��1�յ�Ŀǰ����������Χ0-365*/
    int tm_isdst; /*�չ��Լʱ������*/
};
#endif

time_t str2tm(char *strd)
{
    int y, m, d;
    struct tm t;

    sscanf(strd, "%d-%d-%d", &y, &m, &d);
    if ( y > 1900 && y < 2010 && m >= 1 && m <= 12 && d >= 1 && d <= 31) {
        memset(&t, 0, sizeof(t));
        t.tm_year = y-1900;
        t.tm_mon = m-1;
        t.tm_mday = d;

        return mktime(&t);
    }
    return 0;
}

int test(void)
{
    time_t timep;
    struct tm *p;

    time(&timep);

    p = localtime(&timep);
    printf("time() : %d, %s %d-%d-%d\n", (int)timep, ctime(&timep), p->tm_year+1900, p->tm_mon+1, p->tm_mday);
    timep = mktime(p);
    printf("time()->localtime()->mktime():%d\n", (int)timep);
    timep += 30*24*3600;
    printf("new time(): %d, %s\n", (int)timep, ctime(&timep));

    timep = str2tm((char *)"1999-11-10");
    printf("tm 1999-11-10, %d\n", (int)timep );
    timep += GAP_SECS;
    printf("new date %s\n", ctime(&timep));
    return 0;
}

char *convert(char *str)
{
    int i;
    char *p;
    time_t ts;
    struct tm *ptm;
    char s_date[20];

    for (i = 0; i < sizeof(g_years)/sizeof(g_years[0]); i++) {
        p = strstr(str, g_years[i]);
        if (p) {
            ts = str2tm(p);
            if (ts > 0) {
                ts += GAP_SECS;
                ptm = localtime(&ts);
                sprintf(s_date, "%04d-%02d-%02d", ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday);
                memcpy(p, s_date, strlen(s_date));
            }
            break;
        }
    }
    fputs(str, outfile);
    return str;
}

int convert_file(const char *in, const char *out)
{
    int same_file = 0;

    if (!in) {
        return -1;
    }

    infile = fopen(in, "rb");
    if (infile == NULL) {
        return -2;
    }

    if (!out) {
        outfile = fopen("test.tmp", "wb");
        same_file = 1;
    } else {
        outfile = fopen(out, "wb");
    }

    if (!outfile) {
        fclose(infile);
        return -3;
    }

    while(!feof(infile)) {
        memset(line, 0x0, sizeof(line));
        fgets(line, sizeof(line)-1, infile);
        //if (!feof(infile))
        convert(line);
    }

    fclose(infile);
    fclose(outfile);

    if (same_file) {
    	  unlink(in);
        rename("test.tmp", in);
    }
    printf("convert %s done!\n", in);

    return 0;
}

int main(int argc, char *argv[])
{
    int ret = -1;

    if (argc != 3) {
        ret = convert_file("q1.gpx", NULL);
        if (ret < 0) {
            ret = convert_file("q2.gpx", NULL);
            if (ret < 0) {
                ret = convert_file("q3.gpx", NULL);
                if (ret < 0) {
                    ret = convert_file("q4.gpx", NULL);
                }
            }
        }
        if (ret < 0) {
            printf("'adjdate file1 file2' convert file1 into file2.\n");
        }
        return ret;
    }

    return convert_file(argv[1], argv[2]);
}

#if 0

struct date {
    int year;
    int month;
    int day;
};
int main(void)
{
    int isPrime(int year);
    int dateDiff(struct date mindate,struct date maxdate);
    struct date mindate,maxdate;
    int days;

    printf("please input the one date:");
    scanf("%i-%i-%i",&mindate.year,&mindate.month,&mindate.day);
    printf("please input other day:");
    scanf("%i-%i-%i",&maxdate.year,&maxdate.month,&maxdate.day);

    days=dateDiff(mindate,maxdate);
    printf("the day is:%d\n",days);
    return 0;
}

/************************************************************************/
/*    �ж����꺯��(4��һ��,100�겻��,400������)                         */
/************************************************************************/
int isPrime(int year)
{
    if ((year%4==0&&year%100!=0)||(year%400==0)) {
        return 1;
    } else {
        return 0;
    }

}

int dateDiff(struct date mindate,struct date maxdate)
{
    int days=0, flag=1;
    const int primeMonth[12]= {31,29,31,30,31,30,31,31,30,31,30,31};
    const int notPrimeMonth[12]= {31,28,31,30,31,30,31,31,30,31,30,31};

    /************************************************************************/
    /*        �����������ں���,��С�����ڸ�mindate,��������ڸ�maxdate     */
    /************************************************************************/
    struct date tmp;
    if ((mindate.year>maxdate.year)|| (mindate.year==maxdate.year&&mindate.month>maxdate.month)||(mindate.year==maxdate.year&&mindate.month==maxdate.month&&mindate.day>maxdate.day)) {
        tmp=mindate;
        mindate=maxdate;
        maxdate=tmp;
    }

    int maxmonth,minmonth;
    /************************************************************************/
    /* ��Ҫ˼·:��2002-8-8   2005-2-22��˵                                  */
    /*   ��2004-8-8---2005-2-22----2005-7-8                                 */
    /*һǰһ��պ�N��,���2005-2-22��2005-7-8������,Ȼ��������*36(5|6)����) */
    /*          2002-9-8      2005-11-22                                    */
    /*          2002-9-8-----2005-9-8-----2005-11-22(����Ǽ��Ϻ�������)    */
    /*����ж��Ǽӻ��Ǽ���?������С��,���Ǽ�,��������flag��ʾ             */
    /************************************************************************/
    if (maxdate.month<mindate.month) {
        maxmonth=mindate.month;
        minmonth=maxdate.month;
        flag=-1;
    } else {
        maxmonth=maxdate.month;
        minmonth=mindate.month;
        flag=1;
    }

    /************************************************************************/
    /*  ��mindate.year��ʼ�ۼӵ�maxdate.year                                */
    /************************************************************************/
    for(int j=mindate.year; j<maxdate.year; ++j) {
        if (isPrime(j)==1) {
            days+=366;
        } else
            days+=365;
    }

    /************************************************************************/
    /* ��minmonth�ۼӵ�maxmonth,�������ƽ��                                */
    /************************************************************************/
    int day;
    if(isPrime(maxdate.year)==1) {

        for(int i=minmonth; i<maxmonth; i++) {
            day=primeMonth[i-1]*flag;
            days=days+day;
        }
        days=days+maxdate.day-mindate.day;
    } else {
        for (int i=minmonth; i<maxmonth; i++) {
            day=notPrimeMonth[i-1]*flag;
            days=days+day;
        }
        days=days+maxdate.day-mindate.day;
    }
    return days;
}

#endif
