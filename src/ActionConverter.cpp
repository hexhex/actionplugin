/* dlvhex-dlplugin -- Integration of Answer-Set Programming and Description Logics. 
 * 
 * Copyright (C) 2007, 2008 DAO Tran Minh 
 *  
 * This file is part of dlvhex-dlplugin. 
 * 
 * dlvhex-dlplugin is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version. 
 * 
 * dlvhex-dlplugin is distributed in the hope that it will be useful, but 
 * WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
 * General Public License for more details. 
 * 
 * You should have received a copy of the GNU General Public License 
 * along with dlvhex-dlplugin; if not, write to the Free Software Foundation, 
 * Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA 
 */ 
 
/** 
 * @file   ActionConverter.cpp 
 * @author DAO Tran Minh 
 * @date   Tue Dec 18 17:20:24 2007 
 *  
 * @brief  A converter which converts default rules to dl-rules. 
 *  
 *  
 */ 

#include <iostream>
#include <string>
#include <sstream>
#include "ActionConverter.h"
#include <boost/regex.hpp>
#include <vector>

namespace dlvhex {
namespace aa {

  ActionConverter::ActionConverter()
  {
  }
 
  void
  ActionConverter::convert(std::istream& is, std::ostream& os)
  {
    std::string line, in;
    int index;

    while (!is.eof())
    {
		std::getline(is, line);
		index=line.find('%');
		if (index!=std::string::npos)
		{
			// @bug: this kills strings with % inside (e.g., as in examples/tests/example_group_meeting)
			in+=line.substr(0, index);
		}
		else
			in+=line;
		in+="\n";
    }
	

    std::istringstream input_san_check(in);
    char* a_line2= new char[in.size()];

	while(input_san_check.getline(a_line2, in.size(),'.'))
	{	
		std::string a_line= a_line2;
		int index_fact=a_line.find(":-");
		int index_weak=a_line.find(":~");
		int index_dp;
		if(index_fact<index_weak)
			index_dp=index_weak;
		else
			index_dp=index_fact;
		int index_sharp=a_line.find('#');
		if((index_dp!=std::string::npos && index_sharp!=std::string::npos) && index_dp<=index_sharp)
		{	
			//TODO what if the body contains a action atom???
			//os<<"#error.";
			//std::cout<<"salak error"<<std::endl;
		}

	}
	delete a_line2;

	std::string in_save=in;
	std::string ident = "[a-z_][A-Za-z0-9_]*";
	std::string option = "b|c|c_p";
	std::string variable= "[A-Z][a-zA-Z0-9_]*";
	std::string number= "[0-9]";
	
	boost::smatch pos_body;
	boost::smatch weak_aa;
	std::vector<std::string> weak_aalist;
	const boost::regex e_withwl ("[\\t ]*#("+ident+")\\s*(\\[(.*?)\\])?\\s*\\{\\s*("+option+")\\s*(,\\s*(\\d+|"+variable+"))?\\s*\\}(\\s*\\[\\s*(\\d*|"+variable+")?\\s*:\\s*(\\d*|"+variable+")?\\s*\\][\\t ]*)");
	const boost::regex e_body ("\\s*(:\\s*-(.*?))?\\.\\s*");
	const boost::regex e2 ("[\\t ]*\\s*#("+ident+")\\s*(\\[(.*?)\\])?\\s*\\{\\s*("+option+")\\s*(,\\s*(\\d+|"+variable+"))?\\s*\\}([\\t ]*\\[\\s*(\\d*|"+variable+")?\\s*:\\s*(\\d*|"+variable+")?\\s*\\])?[\\t ]*");
	//[A-Za-z0-9_/\-\",\\s]
	


//TODO if no body but a level and weight exists. ?? possibly done
	if(boost::regex_search(in, pos_body, e_body))
	{
		while (boost::regex_search(in,weak_aa,e_withwl))
		{	
		
			std::string my_match(weak_aa[0].first, weak_aa[0].second);
			if(pos_body[1]!="")
				in_save+="\n:~ "+my_match+", "+pos_body[2]+"."+weak_aa[7];
			else
				in_save+="\n:~ "+my_match+"."+weak_aa[7];
			
			int index= in.find(my_match);
			if(!index)
			{
				std::string temp=in.substr(0, index);
				std::string temp2=in.substr(index+my_match.size());
				in=temp+temp2;
			}
			
			else
			{
				in=in.substr(my_match.size()+1);
			}
			
		}
	}
	in=in_save;
//	std::cout<<"program after weaks"<<in_save<<std::endl;
//TODO verilen programda week constraint varsa

	


	boost::smatch result;
	while( 	boost::regex_search (in, result, e2))
	{
		std::string out_str = " action2009"+result[1]+"(";
		if(result[3]!="")
			out_str=out_str+result[3]+",";
	
		out_str=out_str+result[4]+",";
		if(result[6]!="")
			out_str+=result[6]+",";
		else
			out_str+="0,";
		if(result[7]!="")
		{
			if(result[8]!="")
				out_str+=result[8]+",";
			else
				out_str+="1,";
			if (result[9]!="")
				out_str+=result[9]+") ";
			else
				out_str+="1) ";
		
		}	
		else
			out_str+="0,0) ";

		const boost::regex res0_crrtn ("([\\(\\)\\{\\}\\[\\]])");
		std::string matched_aa = result[0];
		std::string replace_candi= "\\\\\\1";
		matched_aa= boost::regex_replace(matched_aa, res0_crrtn, replace_candi , boost::match_default);
		const boost::regex e (matched_aa,boost::regex::perl);
		in = boost::regex_replace(in, e, out_str,  boost::match_default);
		
	}
	
	os<<in;

//	std::cout<<in<<std::endl; 
	
  }
  
} // namespace aa
} // namespace dlvhex
