/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		               Kefan-Zheng,  2021   		
++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
//数学计算库
//a的b次方
int pow(int a, int b)
{
    int ans = a;
    if(b == 0)
    {
        ans = 1;
        return ans;
    }
    else
    {
        for(int i=0;i<b-1;i++)
        {
            ans = ans*a;
        }
        return ans;
    }
    return ans;
}

//a的绝对值
int abs(int a)
{
    return (a>=0)?a:-a;
}

//字符数组转整形
int atoi(char *str)
{
    int i;
    int n = 0;
    int sign = 1;
    int len = 0;
    for(int j = 0;;j++)
    {
        if(str[j] == '\0')
        {
            break;
        }
        len++;
    }
    for(i=0;i<len;i++)//跳过空白符;
    {
        if(str[i] == ' ')
        {
            continue;
        }
        else if(str[i]=='-')//跳过符号
        {
            sign=(str[i]=='-')?-1:1;
        }
        else
        {
            n=10*n+(str[i]-48);//将数字字符转换成整形数字
        }
    }
    return sign*n;
}