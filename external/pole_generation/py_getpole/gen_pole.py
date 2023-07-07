
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
"#include \"pexsi/getPole.hpp\" "+ newline + \
""+ newline +\
"   poleClass::poleClass(){ "+ newline 

# print(complex_struct)
CPP_CONTENT_REST =  \
"   };"+ newline + \
"   poleClass::~poleClass() {"+ newline + \
"       for (int i = 0; i < size; i++) {"+ newline + \
"          delete[] zshift[i];"+ newline + \
"          delete[] zweight[i];"+ newline + \
"          delete[] fweight[i];"+ newline + \
"          delete[] eweight[i];"+ newline + \
"      }"+ newline + \
"  };"+ newline + \
""+ newline + \
"   // give me a set of parameter, return a set of pole and zshift."+ newline + \
"   bool poleClass::getPole ( int inputMethod, int inputPole, double inputBeta,  " + \
"std::vector< std::complex<double> > &out_zshift, std::vector < std::complex<double> > &out_zweight, " + \
"std::vector< std::complex<double> > &out_fweight, std::vector < std::complex<double> > &out_eweight  ){"+ newline + \
"        for ( int i = 0; i < size; i++ ){"+ newline + \
"            if(inputMethod == method[i] ){ "+ newline + \
"               if(numPole[i] >= inputPole && beta[i] >= inputBeta){"+ newline + \
"                 if(i < size - 1)" + newline + \
"                   if((numPole[i+1] >= inputPole) && (beta[i+1] >= inputBeta)){" + newline + \
"                      // assume the numPole and beta are in assending or desending order " + newline + \
"                      if( (numPole[i+1] < numPole[i]) || (beta[i+1] < beta[i]))" + newline + \
"                          continue;" +newline +\
"                   }" + newline+ \
"                    std::vector<std::complex<double>> tmp_zshift;" + newline + \
"                    std::vector<std::complex<double>> tmp_zweight;" + newline + \
"                    std::vector<std::complex<double>> tmp_fweight;" + newline + \
"                    std::vector<std::complex<double>> tmp_eweight;" + newline + \
"                    for (int k = 0; k < numPole[i]; k++)" + newline + \
"                    {"  + newline + \
"                        tmp_zshift.push_back(std::complex<double>(zshift[i][k].x, zshift[i][k].y));"  + newline + \
"                        tmp_zweight.push_back(std::complex<double>(zweight[i][k].x, zweight[i][k].y));" + newline + \
"                    }" + newline + \
"                   out_zshift = tmp_zshift;"+ newline + \
"                   out_zweight = tmp_zweight;"+ newline + \
"                   if( inputMethod != 2) {  "+ newline + \
"                        for (int k = 0; k < numPole[i]; k++)" + newline + \
"                        {" + newline + \
"                            tmp_fweight.push_back(std::complex<double>(fweight[i][k].x, fweight[i][k].y));" + newline + \
"                            tmp_eweight.push_back(std::complex<double>(eweight[i][k].x, eweight[i][k].y));" + newline + \
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
            zshift_tmp = " ".join("{ " + str(zshift_x[i]) + ", " + str(zshift_y[i]) + " }, \n" + tab5 for i in range(len(zshift_x)))
            zshift += "new Complex[" + str(numPole[-1]) + "]{\n" + tab5 +zshift_tmp + "}, \n" + tab5
            zweight_tmp = " ".join("{ " + str(zweight_x[i]) + ", " + str(zweight_y[i]) + " }, \n" + tab5 for i in range(len(zweight_x)))
            zweight += "new Complex[" + str(numPole[-1]) + "]{\n" + tab5 +zweight_tmp + "}, \n" + tab5
            fweight_tmp = " ".join("{ " + str(fweight_x[i]) + ", " + str(fweight_y[i]) + " }, \n" + tab5 for i in range(len(fweight_x)))
            fweight += "new Complex[" + str(numPole[-1]) + "]{\n" + tab5 +fweight_tmp + "}, \n" + tab5
            eweight_tmp = " ".join("{ " + str(eweight_x[i]) + ", " + str(eweight_y[i]) + " }, \n" + tab5 for i in range(len(eweight_x)))
            eweight += "new Complex[" + str(numPole[-1]) + "]{\n" +tab5 + eweight_tmp + "}, \n" + tab5
    tmp_size = size
    size = tab3 + "size = " + str(size) + ";" + newline
    method = ', '.join(str(item) for item in method)
    method = tab3 + "const int method1 [] = { " + method + " }; " + newline + tab3 + "method.assign(method1, method1 + size);\n"
    numPole = ', '.join(str(item) for item in numPole)
    numPole = tab3 + "const int numPole1 []= { " + numPole + " }; " + newline + tab3 + "numPole.assign(numPole1, numPole1 + size);\n"
    betaEnergy = ', '.join(str(item) for item in betaEnergy)
    betaEnergy = tab3 + "const double betaEnergy1[] = { " + betaEnergy + " }; " + newline + tab3 + "beta.assign(betaEnergy1, betaEnergy1 + size);\n"
    # zshift = ",".join("{ " + str(zshift_x[i]) + ", " + str(zshift_y[i]) + " }\n" + tab5 for i in range(len(zshift_x)))
    zshift = tab3 + "zshift = new Complex*[" + str(tmp_size) + "]" + "{ \n"  + tab5 + zshift + " };" + newline
    # zweight = ",".join("{ " + str(zweight_x[i]) + ", " + str(zweight_y[i]) + " }\n" + tab5 for i in range(len(zweight_x)))
    zweight = tab3 + "zweight = new Complex*[" + str(tmp_size) + "]" + "{ \n" + tab5 + zweight + " };" + newline
    # fweight = ",".join("{ " + str(fweight_x[i]) + ", " + str(fweight_y[i]) + " }\n" + tab5 for i in range(len(fweight_x)))
    fweight = tab3 + "fweight = new Complex*[" + str(tmp_size) + "]" + "{ \n" + tab5 + fweight + " };" + newline
    # eweight = ",".join("{ " + str(eweight_x[i]) + ", " + str(eweight_y[i]) + " }\n" + tab5 for i in range(len(eweight_x)))
    eweight = tab3 + "eweight = new Complex*[" + str(tmp_size) + "]" + "{ \n" + tab5 + eweight + " };" + newline
    print(tab2 + "{" ,newline, size,numPole, method, betaEnergy, newline, zshift, zweight, fweight, eweight + tab2+"}",newline)
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
# print(CPP_TEST_CODE)
# print(type(data[0]))
