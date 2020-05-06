////////////////////////////////////////////////////////////////////////////////////////
// Description: This tool is used to flash new VPD data to eeprom binary file.        //
//              The VPD data flashed by this tool includes MAC0, MAC1,                //
//              PartNumber, SerialNumber, and PrettyName, which are parsed            //
//              according to the data in each field in the XML file and               //
//              flashed to the eeprom binary file.                                    //
// Author:   Andy YF Wang (Andy_YF_Wang@wistron.com)                                  // 
// Date  :   2020/05/06                                                               //
// Ver.  :   1.0.0                                                                    //
////////////////////////////////////////////////////////////////////////////////////////

#include "wEE.hpp"

#include <bits/stdc++.h>
#include <fcntl.h>
#include <libxml/parser.h> // For pasering xml file.
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h> // Includes explicit declaration of lseek.

#include <iostream>
#include <string>

int main(void)
{
    // Open eeprom file.
    openBinFile();

    // Paser the xml file.
    paserXML();

    if (fd)
    {
        // Write the data.
        rwPrettyName(pName_addr_0);
        rwPrettyName(pName_addr_1);
        rwPartNumber(pNumber_addr);
        rwSerialNumber(sNumber_addr);
        rwMAC(mac0_addr, vpdMAC0);
        rwMAC(mac1_addr, vpdMAC1);
    }

    // Close the file.
    close(fd);

    return 0;
}

// Open binary file of eeprom.
void openBinFile()
{
    fd = open("/home/andy/writeEEPROM/eeprom", O_RDWR);
    if (fd < 0)
    {
        close(fd);
    }
}

// Write the data of PrettyName.
void rwPrettyName(int addr)
{
    int len, size;
    char* bufw = const_cast<char*>(vpdPTN.c_str());

    // Data lengh.
    len = strlen(bufw);

    if (!fd)
    {
        openBinFile();
    }

    // Position first and then write data.
    lseek(fd, addr, SEEK_SET);
    if ((size = write(fd, bufw, len)) < 0)
    {
        close(fd);
    }
}

// Write the data of PartNumber.
void rwPartNumber(int addr)
{
    int len, size;
    char* bufw = const_cast<char*>(vpdPN.c_str());

    // Data lengh.
    len = strlen(bufw);

    if (!fd)
    {
        openBinFile();
    }

    // Position first and then write data.
    lseek(fd, addr, SEEK_SET);
    if ((size = write(fd, bufw, len)) < 0)
    {
        close(fd);
    }
}

// Write & Read the data of SerialNumber.
void rwSerialNumber(int addr)
{
    int len, size;
    char* bufw = const_cast<char*>(vpdSN.c_str());

    // Data lengh.
    len = strlen(bufw);

    if (!fd)
    {
        openBinFile();
    }

    // Position first and then write data.
    lseek(fd, addr, SEEK_SET);
    if ((size = write(fd, bufw, len)) < 0)
    {
        close(fd);
    }
}

// Write the data of MAC.
// Because the MAC data parsed from the XML file
// is a string, if the function writes this string
// directly to the eeprom binary, there will be
// errors in the data (the data written to the eeprom
// binary must be in ASCII format), so this function
// need to separate this string and write separately.
void rwMAC(int addr, string data)
{
    int size;
    string hvalue0 = data.substr(0, 2);
    string hvalue1 = data.substr(2, 2);
    string hvalue2 = data.substr(4, 2);
    string hvalue3 = data.substr(6, 2);
    string hvalue4 = data.substr(8, 2);
    string hvalue5 = data.substr(10, 2);

    int m = hexToASCII(hvalue0).size();
    int n = hexToASCII(hvalue1).size();
    int o = hexToASCII(hvalue2).size();
    int p = hexToASCII(hvalue3).size();
    int q = hexToASCII(hvalue4).size();
    int r = hexToASCII(hvalue5).size();

    char bufw0[m + 1];
    char bufw1[n + 1];
    char bufw2[o + 1];
    char bufw3[p + 1];
    char bufw4[q + 1];
    char bufw5[r + 1];

    strcpy(bufw0, hexToASCII(hvalue0).c_str());
    strcpy(bufw1, hexToASCII(hvalue1).c_str());
    strcpy(bufw2, hexToASCII(hvalue2).c_str());
    strcpy(bufw3, hexToASCII(hvalue3).c_str());
    strcpy(bufw4, hexToASCII(hvalue4).c_str());
    strcpy(bufw5, hexToASCII(hvalue5).c_str());

    if (!fd)
    {
        openBinFile();
    }

    // Position first and then write data separately.
    lseek(fd, addr, SEEK_SET);
    if ((size = write(fd, bufw0, m)) < 0)
    {
        close(fd);
    }

    lseek(fd, addr + 1, SEEK_SET);
    if ((size = write(fd, bufw1, n)) < 0)
    {
        close(fd);
    }

    lseek(fd, addr + 2, SEEK_SET);
    if ((size = write(fd, bufw2, o)) < 0)
    {
        close(fd);
    }

    lseek(fd, addr + 3, SEEK_SET);
    if ((size = write(fd, bufw3, p)) < 0)
    {
        close(fd);
    }

    lseek(fd, addr + 4, SEEK_SET);
    if ((size = write(fd, bufw4, q)) < 0)
    {
        close(fd);
    }

    lseek(fd, addr + 5, SEEK_SET);
    if ((size = write(fd, bufw5, r)) < 0)
    {
        close(fd);
    }
}

// hex-to-ascii c++
string hexToASCII(string hex)
{
    // Initialize the ASCII code string as empty.
    string ascii_value = "";
    for (size_t i = 0; i < hex.length(); i += 2)
    {
        // Extract two characters from hex string.
        string part_value = hex.substr(i, 2);

        // Change it into base 16 and typecast as the character.
        char ch = std::stoul(part_value, nullptr, 16);

        // Add this char to final ASCII string.
        ascii_value += ch;
    }
    return ascii_value;
}

// Paser the XML file.
void paserXML()
{
    xmlDocPtr doc;
    xmlNodePtr curNode;
    xmlChar* keyMAC0; // MAC0 address
    xmlChar* keyMAC1; // MAC1 address
    xmlChar* keyPN;   // PartNumber
    xmlChar* keySN;   // SerialNumber
    xmlChar* keyPTN;  // PrettyName

    char szDocName[] = "/usr/bin/writeEEPROM/vpddata.xml";

    // Paser the xml file.
    doc = xmlReadFile(szDocName, "UTF-8", XML_PARSE_RECOVER);
    if (NULL == doc)
    {
        cerr << "Paser the xml file fail!" << endl;
    }

    curNode = xmlDocGetRootElement(doc);
    if (NULL == curNode)
    {
        xmlFreeDoc(doc);
    }

    if (xmlStrcmp(curNode->name, BAD_CAST "record"))
    {
        xmlFreeDoc(doc);
    }

    curNode = curNode->xmlChildrenNode;

    while (curNode != NULL)
    {
        // Get the data of MAC0.
        if ((!xmlStrcmp(curNode->name, (const xmlChar*)"keyword1")))
        {
            keyMAC0 = xmlNodeGetContent(curNode);
            string mac0(reinterpret_cast<char*>(keyMAC0));
            vpdMAC0 = mac0;
            xmlFree(keyMAC0);
        }

        // Get the data of MAC1.
        if ((!xmlStrcmp(curNode->name, (const xmlChar*)"keyword2")))
        {
            keyMAC1 = xmlNodeGetContent(curNode);
            string mac1(reinterpret_cast<char*>(keyMAC1));
            vpdMAC1 = mac1;
            xmlFree(keyMAC1);
        }

        // Get the data of PN.
        if ((!xmlStrcmp(curNode->name, (const xmlChar*)"keyword3")))
        {
            keyPN = xmlNodeGetContent(curNode);
            string pn(reinterpret_cast<char*>(keyPN));
            vpdPN = pn;
            xmlFree(keyPN);
        }

        // Get the data of SN.
        if ((!xmlStrcmp(curNode->name, (const xmlChar*)"keyword4")))
        {
            keySN = xmlNodeGetContent(curNode);
            string sn(reinterpret_cast<char*>(keySN));
            vpdSN = sn;
            xmlFree(keySN);
        }

        // Get the data of PTN.
        if ((!xmlStrcmp(curNode->name, (const xmlChar*)"keyword5")))
        {
            keyPTN = xmlNodeGetContent(curNode);
            string ptn(reinterpret_cast<char*>(keyPTN));
            vpdPTN = ptn;
            xmlFree(keyPTN);
        }

        curNode = curNode->next;
    }
    xmlFreeDoc(doc);
}
