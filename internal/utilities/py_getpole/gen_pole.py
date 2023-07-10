
import sys

data = []
tab       = "    "
tab2      = "        "
tab3      = "            "
tab5      = "                                       "
newline   = "\n"
complex_struct = "struct Complex {\n" \
    + tab + "double x;\n" \
    + tab + "double y; \n" \
"};\n"
include_header =  \
"#include \"pexsi/environment.hpp\" "+ newline + \
"#include \"pexsi/getPole.hpp\" "+ newline 

# print(complex_struct)
CPP_CONTENT_REST =  \
"void sum_prefix()" + newline + \
"{" + newline + \
"    sum[0] = 0;" + newline + \
"    for (int i = 0; i < size1; i ++ )" + newline + \
"    {" + newline + \
"        sum[i + 1] += sum[i] + numPole[i];" + newline + \
"    }" + newline + \
"}" + newline + \
"   poleClass::poleClass() {" + "}" + ";"+ newline + \
"   poleClass::~poleClass() {" + "}" + ";"+ newline + \
""+ newline + \
"   // give me a set of parameter, return a set of pole and zshift."+ newline + \
"bool poleClass::getPole ( int inputMethod, int inputPole, double inputBeta,  " + \
"std::vector< std::complex<double> > &out_zshift, std::vector < std::complex<double> > &out_zweight, " + \
"std::vector< std::complex<double> > &out_fweight, std::vector < std::complex<double> > &out_eweight  ){"+ newline + \
"    sum_prefix();"+ newline + \
"        for ( int i = 0; i < size1; i++ ){"+ newline + \
"            if(inputMethod == method[i] ){ "+ newline + \
"               if(numPole[i] >= inputPole && beta[i] >= inputBeta){"+ newline + \
"                 if(i < size1 - 1)" + newline + \
"                   if((numPole[i+1] >= inputPole) && (beta[i+1] >= inputBeta)){" + newline + \
"                      // assume the numPole and beta are in assending or desending order " + newline + \
"                      if( (numPole[i+1] < numPole[i]) || (beta[i+1] < beta[i]))" + newline + \
"                          continue;" +newline +\
"                   }" + newline+ \
"                    std::vector<std::complex<double>> tmp_zshift;" + newline + \
"                    std::vector<std::complex<double>> tmp_zweight;" + newline + \
"                    std::vector<std::complex<double>> tmp_fweight;" + newline + \
"                    std::vector<std::complex<double>> tmp_eweight;" + newline + \
"                    for (int k = sum[i]; k < sum[i + 1]; k++)" + newline + \
"                    {"  + newline + \
"                        tmp_zshift.push_back(std::complex<double>((double)zshift[k][0], (double)zshift[k][1]));"  + newline + \
"                        tmp_zweight.push_back(std::complex<double>((double)zweight[k][0], (double)zweight[k][1]));" + newline + \
"                    }" + newline + \
"                   out_zshift = tmp_zshift;"+ newline + \
"                   out_zweight = tmp_zweight;"+ newline + \
"                   if( inputMethod != 2) {  "+ newline + \
"                        for (int k = sum[i]; k < sum[i + 1]; k++)" + newline + \
"                        {" + newline + \
"                            tmp_fweight.push_back(std::complex<double>((double)eweight[k][0], (double)eweight[k][1]));" + newline + \
"                            tmp_eweight.push_back(std::complex<double>((double)fweight[k][0], (double)fweight[k][1]));" + newline + \
"                        }" + newline + \
"                        out_fweight = tmp_fweight;" + newline + \
"                        out_eweight = tmp_eweight;" + newline + \
"                   }                        "+ newline + \
"                   return true;"+ newline + \
"               }"+ newline + \
"            }"+ newline + \
"        }"+ newline + \
"        return false;"+ newline + \
"   }"+ newline + \
"   "+ newline 

CPP_TEST_CODE = \
"int main()" + newline + \
"{" + newline + \
"    poleClass pole1;" + newline + \
"    std::vector< std::complex<double> > zshift;" + newline + \
"    std::vector< std::complex<double> > zweight;" + newline + \
"    std::vector< std::complex<double> > fweight;" + newline + \
"    std::vector< std::complex<double> > eweight;" + newline + \
"    pole1.getPole(2, 10,10, zshift, zweight, fweight, eweight);" + newline + \
"" + newline + \
"    std::cout << \" method is \" << 2 << \" beta is :\" << 10 << \" numPole \" << 10 << std::endl; " + newline  + \
"    std::cout << std::endl<< \" zshift returns:\" << std::endl;" + newline + \
"    for(int i = 0; i < zshift.size(); i++)" + newline + \
"       std::cout<< std::setw(10) << zshift[i].real() << std::setw(10) << zshift[i].imag() << std::endl;" + newline + \
"    pole1.getPole(1, 20,20, zshift, zweight, fweight, eweight);" + newline + \
"" + newline + \
"    std::cout << \" method is \" << 1 << \" beta is :\" << 20 << \" numPole \" << 20 << std::endl;" + newline + \
"    std::cout << std::endl<< \" zshift returns:\" << std::endl;" + newline + \
"    for(int i = 0; i < zshift.size(); i++)" + newline + \
"       std::cout<< std::setw(10) << zshift[i].real() << std::setw(10) << zshift[i].imag() << std::endl;" + newline + \
"    return 0;" + newline + \
"}  " 

def readnumbers():
    """ read the PoleData13.dat and split them into list data """
    try:
        with open("PoleData123.dat", "r") as infile:
            for line in infile:
                data.append(list(filter(None, line.strip('\n').split(' '))))
    except:
        pass
    return data



# this is the main function of the pole.py
def constructPoleCPP():
    """ construct the wx, wy, zx, zy  then write them to file"""
    method = [];numPole = [];betaEnergy = []
    zshift = str("")
    zweight = str("")
    fweight= str("")
    eweight = str("")
    size = 0
    for i in range(len(data)):
        if len(list(data[i])) == 3:  # control line
            method.append(data[i][0])
            numPole.append(int(data[i][1]))
            betaEnergy.append(data[i][2])

            zshift_x = [];zshift_y = []; zweight_x = [];zweight_y = []
            fweight_x =[]; fweight_y =[]; eweight_x = []; eweight_y = []
            for j in range(numPole[-1]):
                zshift_x.append(data[i + j + 1][0])
                zshift_y.append(data[i + j + 1][1])
                zweight_x.append(data[i + j + 1][2])
                zweight_y.append(data[i + j + 1][3])
                # print(method[-1])
                if int(len(data[i + j + 1])) > 4:
                    fweight_x.append(data[i + j + 1][4])
                    fweight_y.append(data[i + j + 1][5])
                    eweight_x.append(data[i + j + 1][6])
                    eweight_y.append(data[i + j + 1][7])
            size += 1
            zshift_tmp = "".join("{ " + str(zshift_x[i]) + ", " + str(zshift_y[i]) + " }, \n" + tab for i in range(len(zshift_x)))
            zshift +=  zshift_tmp
            zweight_tmp = "".join("{ " + str(zweight_x[i]) + ", " + str(zweight_y[i]) + " }, \n" + tab for i in range(len(zweight_x)))
            zweight += zweight_tmp 
            fweight_tmp = "".join("{ " + str(fweight_x[i]) + ", " + str(fweight_y[i]) + " }, \n" + tab for i in range(len(fweight_x)))
            fweight += fweight_tmp
            eweight_tmp = "".join("{ " + str(eweight_x[i]) + ", " + str(eweight_y[i]) + " }, \n" + tab for i in range(len(eweight_x)))
            eweight += eweight_tmp 
    tmp_size = size
    size = "#define size1 " + str(size) + newline
    sum = "int sum[size1 + 1];" + newline
    method = ', '.join(str(item) for item in method)
    method = "const int method [] = { " + method + " }; " + newline 
    numPole = ', '.join(str(item) for item in numPole)
    numPole = "const int numPole []= { " + numPole + " }; " + newline 
    betaEnergy = ', '.join(str(item) for item in betaEnergy)
    betaEnergy = "const double beta[] = { " + betaEnergy + " }; " + newline
    # zshift = ",".join("{ " + str(zshift_x[i]) + ", " + str(zshift_y[i]) + " }\n" + tab5 for i in range(len(zshift_x)))
    zshift = "const double zshift[][2] = " + "{ \n"  + tab + zshift + " };" + newline
    # zweight = ",".join("{ " + str(zweight_x[i]) + ", " + str(zweight_y[i]) + " }\n" + tab5 for i in range(len(zweight_x)))
    zweight =  "const double zweight[][2] = " + "{ \n"  + tab  + zweight + " };" + newline
    # fweight = ",".join("{ " + str(fweight_x[i]) + ", " + str(fweight_y[i]) + " }\n" + tab5 for i in range(len(fweight_x)))
    fweight =  "const double fweight[][2] = " + "{ \n"  + tab  + fweight + " };" + newline
    # eweight = ",".join("{ " + str(eweight_x[i]) + ", " + str(eweight_y[i]) + " }\n" + tab5 for i in range(len(eweight_x)))
    eweight = "const double eweight[][2] = " + "{ \n"  + tab  + eweight + " };" + newline
    print(tab2 + newline, size, sum, numPole, method, betaEnergy, newline, zshift, zweight, fweight, eweight ,newline)
# readnumbers()
# print include_header
# constructPoleCPP()  
zshift_x = [1, 2, 3]  
zshift_y = [1, 2, 3]
zshift = ", ".join("{ " + str(zshift_x[i]) + ", " + str(zshift_y[i]) + " }" for i in range(len(zshift_x)))
zshift = tab2 + "const Complex zshift = { " + zshift + " };" + newline
# print(zshift)
readnumbers()
print(include_header)
constructPoleCPP()
print(CPP_CONTENT_REST)
print(CPP_TEST_CODE)
# print(type(data[0]))
