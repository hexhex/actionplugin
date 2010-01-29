/* dlvhex -- Answer-Set Programming with external interfaces.
 * Copyright (C) 2008 DAO Tran Minh
 * 
 * This file is part of dlvhex-dlplugin.
 *
 * dlvhex is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * dlvhex is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with dlvhex; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 */


/**
 * @file   ActionOutputBuilder.cpp
 * @author DAO Tran Minh
 * @date   Fri Dec 12 16:24:00 CET 2008
 * 
 * @brief  Builders for default rules output.
 * 
 * 
 */

#include "ActionOutputBuilder.h"
#include "AddonAtom.h"
#include "AddonContainer.h"
#include "dlvhex/AnswerSet.h"
#include <string>
#include <algorithm>
#include <vector>
#include "dlvhex/globals.h"

namespace dlvhex {
namespace aa {


   bool Exists(const std::vector<AtomPtr> & a_vec, const AtomPtr candi)
   {	
	for(int i=0; i<a_vec.size(); i++)
	{
		//std::cout<< "current atom is: " << a_vec[i]->getArgument(4).getInt()<<" ";
		//std::cout << "candi is: " << (*candi).getArgument(4).getInt();
		//std::cout << "in exists i is: " << i << "\n";
		if(*a_vec[i]==*candi)
		{	
			
			//std::cout << "in exists just about to return true;\n";
			return true;
		}	
	}
	//std::cout << "in exists just about to return false;\n";
	return false;

   }

   ActionOutputBuilder::ActionOutputBuilder()
  { 
	rc_intersection=NULL;
	best_intersection=NULL;
  }

  ActionOutputBuilder::~ActionOutputBuilder()
  { 
	delete rc_intersection;
	delete best_intersection;
  }  



  struct AtomComparer
  {
	bool
	operator() (dlvhex::AtomPtr aa1,dlvhex::AtomPtr aa2)
	{
		////std::cout << "comparing two atomptrs\n";
		bool ret=((*aa1)<(*aa2));
		////std::cout << "compared two atomptrs\n";		
		return ret;
	}
  } comparatorObject;

  struct PrecedenceComparer
  {
	bool
	operator() (dlvhex::AtomPtr aa1,dlvhex::AtomPtr aa2)
	{	//std::cout<<"first prec is"<< (aa1->getArgument((aa1->getArity())-2))<<std::endl;
		//std::cout<<"second prec is"<< (aa2->getArgument((aa2->getArity())-2))<<std::endl;
		return (aa1->getArgument((aa1->getArity())-2))<(aa2->getArgument((aa2->getArity())-2));
	}
  } comparatorObject2;

  void
  ActionOutputBuilder::buildResult(std::ostream& stream, const dlvhex::ResultContainer& facts)
  {
    const dlvhex::ResultContainer::result_t& results = facts.getAnswerSets();
	dlvhex::RawPrintVisitor rpv(stream);
    if (!Globals::Instance()->getOption("Silent"))
    {
      stream << std::endl;
    }
   
    if (results.empty())
    {
	return;
    }


    if (((*results.begin())->hasWeights()) && !dlvhex::Globals::Instance()->getOption("AllModels"))
    {
      stream << "Best model: ";
    }
  
    std::string p1 = "action2009";
    //std::string p2 = PREFIX_IN;
    //std::list<std::string> cnames = ActionProcessor::defaults.getConclusionsNames();
    for (dlvhex::ResultContainer::result_t::const_iterator rit = results.begin(); rit != results.end(); ++rit)
    {
	      stream<<"{";	
	      AnswerSet* newASPtr;
		if((*rit)->hasWeights())
		{      
			newASPtr=new AnswerSet("action2009");//AnswerSet new_as("action2009");			
			//std::cout<<"existing as has weight\n";
			for(int s=1; s<=(*rit)->getMaxLevel(); s++)
		    {
//			std::cout<<"old as has weight:"<<(*rit)->getWeight(s)<<" at level "<<s<<"\n";
				newASPtr->addWeight((*rit)->getWeight(s), s);
//			std::cout<<"new as has weight:"<<new_as.getWeight(s)<<" at level "<<s<<"\n";
			}
                }
		else
		{
			newASPtr=new AnswerSet;
			//std::cout << "existing as has no weight\n";
		}
		
		if(newASPtr->hasWeights())
			std::cout<<"new as has weight\n";

	      for (dlvhex::AnswerSet::const_iterator i = (*rit)->begin(); i != (*rit)->end(); )
	      {
					//std::cout << "selen hasta oluyo\n";
					////std::cout << i->getPredicate().getString() << std::endl;
					////std::cout << i->getArguments() << std::endl;
					std::string pname = i->getPredicate().getString();
					if (pname.find(p1) == 0)
						{
							//create filtered result container
							//Atom aa (new Atom(i->getArgument(0).getString(), i->getArguments()));
							AtomPtr aa_atom(new Atom(i->getArgument(0).getString(), i->getArguments()));
							newASPtr->insert(aa_atom);


						  	std::string newname = pname.substr(p1.length());
							int arity= i->getArity();


							
							int level= i->getArgument(arity).getInt();
							int weight= i->getArgument(arity-1).getInt();
							int prec= i->getArgument(arity-2).getInt();
							std::string opt= i->getArgument(arity-3).getString();  
					
							stream<<"#"<<newname;

							if(arity>4)
							{
								stream<<"[";
								for(int j=1; j<(arity-4); j++)
									{
										stream<< i->getArgument(j)<<",";
									}
								stream<< i->getArgument(arity-4)<<"]";
							}
							stream<<"{"<<opt;
							if(prec!=0)
								stream<<", "<<prec;
							stream<<"}";
							if(weight!=0 && level!=0)		
							{
								stream<<"["<<weight<<":"<<level<<"]";
							}
						  ////std::cout << newname << std::endl;
						//  dlvhex::AtomPtr ap(new Atom(newname, i->getArguments(), true));
					 //   new_as.insert(ap);
						}
					else
					{
					   //dlvhex::AtomPtr ap(new Atom(i->getArgument(0).getString(), i->getArguments()));
				           //new_as.insert(ap);
					   //rpv.visit(&(*i));//
					   dlvhex::Atom a_atom= (*i);
					   a_atom.accept(rpv);
					  
					}
					//dlvhex::ResultContainer::result_t::const_iterator itt =i;
					i++;
					if(i!=(*rit)->end())
						stream<<", ";
				
		}
		
		filtered_rc.push_back(*newASPtr);
		//RawPrintVisitor rpv(std::cout);
		//std::cout << "pushed AS as: ";
		//newASPtr->accept(rpv);



	     	//if((*(filtered_rc.begin())).hasWeights())
		//	std::cout<<"new as has weight2\n";

	      stream <<"}"<< std::endl;
	      if ((*rit)->hasWeights())
		{
		  stream << "Cost ([Weight:Level]): <";
		  
		  //
		  // Display all weight values up to the highest specified level
		  //
		  for (unsigned lev = 1; lev <= AnswerSet::getMaxLevel(); ++lev)
		    {
		      if (lev > 1)
			stream << ",";
		      
		      stream << "[" << (*rit)->getWeight(lev) << ":" << lev << "]";
		    }
		  
		  stream << ">" << std::endl;
		}
	      
	      //
	      // empty line
	      //
	      if (!dlvhex::Globals::Instance()->getOption("Silent"))
		{
		  stream << std::endl;
		}
    }
	
	//filtered_rc.print(std::cout, this);
	
	//std::cout<<"result container's size:"<< filtered_rc.getAnswerSets().size()<< std::endl;
	//std::cout<<"finding intersection\n";


	
	AddonContainer* container = new AddonContainer("");
	AddonInterface::AddonAtomFunctionMap a_fm; 
	std::vector<AddonInterface*> addons = container->importAddons();
	//ctx->addPlugins(plugins);
//	std::cout<< "addons.size()== " << addons.size() << std::endl;

	//sort the atoms in the first answerset
	std::vector <AtomPtr> best_model;
	std::vector<AnswerSet>::const_iterator rit1 = filtered_rc.begin(); 
	for(dlvhex::AnswerSet::const_iterator it= (*rit1).begin(); it != (*rit1).end(); it++)
	{
		dlvhex::AtomPtr ap(new Atom(it->getArgument(0).getString(), it->getArguments()));
		best_model.push_back(ap);
	}	
	
	for(std::vector<AtomPtr>::iterator vit = best_model.begin(); vit!=best_model.end(); ++vit)
	{
//		std::cout<<"current option is: " << (*vit)->getArgument(((*vit)->getArity())-3).getString()<<std::endl;
		std::string option=(*vit)->getArgument(((*vit)->getArity())-3).getString();
		if( option=="b")
		{
//			std::cout<<"brave atom is seen"<<std::endl;//Execute  action
//			//std::cout << "an action atom is to be executed: " << (**vit).getPredicate().getString()<<std::endl;//execute action
		      bool executed=false;
		      for (std::vector<AddonInterface*>::const_iterator pi = addons.begin();
				   pi != addons.end() && !executed; ++pi)
				{
	//std::cout << "inside for"<<std::endl;					
					(*pi)->getAtoms(a_fm);
					for(AddonInterface::AddonAtomFunctionMap::iterator fm_it=a_fm.begin(); fm_it!=a_fm.end(); ++fm_it)
					{
	//std::cout << "inside 2nd for"<<std::endl;					
						std::string atom_name= (*fm_it).first;
	//std::cout << "atom_name: "<<atom_name<<std::endl;
//	//std::cout << "atom_name2: "<<(**vit).getPredicate().getString()<<std::endl;
	
	
						if(atom_name==((**vit).getPredicate().getString()).substr(10))
						{	
//	//std::cout << "found an action atom with name: "<<atom_name<<std::endl;		
							Tuple  argument_list=(**vit).getArguments();
//	std::cout<<"argument list size: "<<argument_list.size()<<std::endl;
							argument_list.pop_back();			
							argument_list.pop_back();
							argument_list.pop_back();
							argument_list.pop_back();
						//	AddonAtom::Query a_query(argument_list);
//std::cout<<"query param size: "<<argument_list.size()<<std::endl;
//std::cout << "1\n";
							(*((*fm_it).second)).execute(argument_list);
////std::cout << "2\n";
							executed=true;
							break;
						}
					}					
				}			
		}
//TODO for a possibly more efficient implementation consider sorting intersections
		else if(option=="c")
			{
				if(rc_intersection==NULL)
				{
					rc_intersection= new std::vector<AtomPtr>; 
					*rc_intersection= findIntersection(filtered_rc);
				}
				if(Exists(*rc_intersection, (*vit)))
				{
					//std::cout << "an action atom is to be executed but not yet implemented: " << (**vit).getPredicate().getString();//execute action
					 bool executed=false;
		      			 for (std::vector<AddonInterface*>::const_iterator pi = addons.begin();pi != addons.end() && !executed; ++pi)
						{
	//std::cout << "inside for"<<std::endl;					
					(*pi)->getAtoms(a_fm);
						for(AddonInterface::AddonAtomFunctionMap::iterator fm_it=a_fm.begin(); fm_it!=a_fm.end(); ++fm_it)
						{
	//std::cout << "inside 2nd for"<<std::endl;					
							std::string atom_name= (*fm_it).first;
	//std::cout << "atom_name: "<<atom_name<<std::endl;
//	//std::cout << "atom_name2: "<<(**vit).getPredicate().getString()<<std::endl;
	
	
							if(atom_name==((**vit).getPredicate().getString()).substr(10))
							{	
//	//std::cout << "found an action atom with name: "<<atom_name<<std::endl;		
								Tuple  argument_list=(**vit).getArguments();
//	std::cout<<"argument list size: "<<argument_list.size()<<std::endl;
								argument_list.pop_back();			
								argument_list.pop_back();
								argument_list.pop_back();
								argument_list.pop_back();
						//	AddonAtom::Query a_query(argument_list);
//std::cout<<"query param size: "<<argument_list.size()<<std::endl;
//std::cout << "1\n";
								(*((*fm_it).second)).execute(argument_list);
////std::cout << "2\n";
								executed=true;
								break;
							}
						}					
					}			
				}
			}
					
		else //opt="cp"
		{	
//				std::cout<<"In opt=cp"<<std::endl;
				std::vector<AnswerSet> best_models;
				if(best_intersection==NULL)
				{
					best_intersection= new std::vector<AtomPtr>; 
					//const dlvhex::ResultContainer::result_t& result_f= filtered_rc.getAnswerSets();
					
					//std::cout << "filteredrc size is: " << filtered_rc.size() << "\n";


					for (std::vector<AnswerSet>::iterator rit4 = filtered_rc.begin(); rit4 != filtered_rc.end(); ++rit4)
					{
						//std::cout << "ritsize before is: " << (*rit4).size() << std::endl;
					}


					std::vector<AnswerSet>::iterator weightedAnswerSetIterator = filtered_rc.begin();
					AnswerSet* firstAnswerSet = &(*weightedAnswerSetIterator);
					//std::cout << "The first answer set's size is: " << (*firstAnswerSet).size() << std::endl;
					//std::cout << "That answer set has weights? Answer is: " << (*firstAnswerSet).hasWeights() << std::endl;
					
					//find all the best models
					
					while(++weightedAnswerSetIterator != filtered_rc.end() &&  !(*(weightedAnswerSetIterator)).cheaperThan(*firstAnswerSet))
					{
						best_models.push_back(*weightedAnswerSetIterator);
						//(*rit3).accept(rpv);
					}
					//std::cout << "There are a total of " << best_models.size() << " best models\n";
					//std::cout << "Finding the intersection of all best models\n";
					*best_intersection=findIntersection(best_models);
					//std::cout << "Found the intersection of all best models\n";
					
				}
				
				if(Exists(*best_intersection, *vit))
				{
					//std::cout << "an atom is to be executed\n";
					bool executed=false;
					for (std::vector<AddonInterface*>::const_iterator pi = addons.begin(); pi != addons.end() && !executed; ++pi)
					{
//						//std::cout << "inside for"<<std::endl;					
						(*pi)->getAtoms(a_fm);
						for(AddonInterface::AddonAtomFunctionMap::iterator fm_it=a_fm.begin(); fm_it!=a_fm.end(); ++fm_it)
						{
//							//std::cout << "inside 2nd for"<<std::endl;					
							std::string atom_name= (*fm_it).first;
//							//std::cout << "atom_name: "<<atom_name<<std::endl;
							//	//std::cout << "atom_name2: "<<(**vit).getPredicate().getString()<<std::endl;
							if(atom_name==((**vit).getPredicate().getString()).substr(10))
							{	
								//	//std::cout << "found an action atom with name: "<<atom_name<<std::endl;		
								Tuple  argument_list=(**vit).getArguments();
								//	std::cout<<"argument list size: "<<argument_list.size()<<std::endl;
								argument_list.pop_back();			
								argument_list.pop_back();
								argument_list.pop_back();
								argument_list.pop_back();
								//	AddonAtom::Query a_query(argument_list);
								//std::cout<<"query param size: "<<argument_list.size()<<std::endl;
								////std::cout << "1\n";
								(*((*fm_it).second)).execute(argument_list);
								////std::cout << "2\n";
								executed=true;
								break;
							}
						}					
					}		
				}
					
					
				
				
				
		}
	}
  }




  std::vector<AtomPtr>
  ActionOutputBuilder::findIntersection(const std::vector<AnswerSet> & rc)
  {
	//const dlvhex::ResultContainer::result_t& results = rc.getAnswerSets();
	//std::set<dlvhex::AtomPtr> intersection;
	std::vector<dlvhex::AtomPtr> intersection;
	std::vector<dlvhex::AtomPtr> intersection2;
	std::vector<AnswerSet>::const_iterator rit = rc.begin();
	std::vector<AnswerSet>::const_iterator rit2 = rc.begin();
	std::vector< std::vector<AtomPtr> > answerSets;
	for(;rit!=rc.end();rit++)
	{
		std::vector<AtomPtr> an_answerSet;
		for(dlvhex::AnswerSet::const_iterator i = (*rit).begin(); i != (*rit).end(); i++)
		{
			AtomPtr an_aptr (new Atom(i->getArgument(0).getString(), i->getArguments()));
			an_answerSet.push_back(an_aptr);
		}
		answerSets.push_back(an_answerSet);
	}
//		std::cout<<"2\n";
	if (answerSets.size()==0)
		return intersection;
		
	else if (answerSets.size()==1)
	{
		std::sort(answerSets[0].begin(), answerSets[0].end(),comparatorObject2);
		return 	answerSets[0];

	}
//	std::cout<<"4\n";
	std::vector<AtomPtr>::iterator vit1b,vit1e,vit2b,vit2e;
	
	vit1b=answerSets[0].begin();
	vit1e=answerSets[0].end();
	
	vit2b=answerSets[1].begin();
	vit2e=answerSets[1].end();

	/*for(std::vector<AtomPtr>::iterator it=answerSets[0].begin(); it!=answerSets[0].end();it++)
	{
		//std::cout << "heyo" << std::endl;
	}
*/
	//std::cout<<"5\n";
	////std::cout << "vit1b is: " << (*vit1b)->getPredicate() << std::endl;
	
	////std::cout << "vit2b is: " << (*vit2b)->getPredicate() << std::endl;
	////std::cout << "vit2e is: " << (*vit2e)->getPredicate() << std::endl;
	////std::cout << "vit1e is: " << (*vit1e)->getPredicate() << std::endl;
	//intersection.push_back(AtomPtr(new Atom("ozan")));
	////std::cout << "printing inter\n";
/*	for(std::vector<AtomPtr>::iterator it=intersection.begin();it!=intersection.end();it++)
	{
		//std::cout << (*it)->getPredicate() << std::endl;
	}

*/
	std::set_intersection(vit1b,vit1e,vit2b,vit2e,std::back_inserter(intersection),comparatorObject);
//	std::cout<<"6\n";
	if(answerSets.size()==2)
	{
//		std::cout<<"9\n";
		std::sort(intersection.begin(), intersection.end(), comparatorObject2);
//		std::cout<<"intersection size just before return: "<< intersection.size()<<"\n";
		
		return 	intersection;
	}
//		std::cout<<"7\n";
	for (int s=3; s<answerSets.size(); s++)
	{
//	std::cout<<"7."<<s<<"\n";
		set_intersection(intersection.begin(),intersection.end(),
				 answerSets[s].begin(), answerSets[s].end(),
				 std::back_inserter(intersection2), comparatorObject);
		intersection= intersection2;
	}
//	std::cout<<"8\n";
	std::sort(intersection.begin(), intersection.end(), comparatorObject2);
//	std::cout<<"9\n";
//	std::cout<<"size is: "<< intersection.size()<<"\n";

	
	return intersection;

  }

} // namespace aa
} // namespace dlvhex
