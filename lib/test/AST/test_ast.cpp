#include <gtest/gtest.h>
#include <veriparse/AST/nodes.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>


using namespace Veriparse;


TEST(ASTTest, Clone) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("ASTTest.Clone.log");
	Logger::add_stdout_sink();

	AST::Parameter::ListPtr params1;
	AST::Node::ListPtr ports1;
	AST::Node::ListPtr items1(new AST::Node::List);

	AST::Parameter::ListPtr params2;
	AST::Node::ListPtr ports2;
	AST::Node::ListPtr items2(new AST::Node::List);

	AST::Node::Ptr mod1(new AST::Module(params1, ports1, items1,
	                                    std::string("module1"), AST::Module::Default_nettypeEnum::WIRE));
	AST::Node::Ptr mod2(new AST::Module(params2, ports2, items2,
	                                    std::string("module2"), AST::Module::Default_nettypeEnum::WIRE));

	AST::Node::ListPtr module_list(new AST::Node::List);
	module_list->push_back(mod1);
	module_list->push_back(mod2);

	AST::Description::Ptr desc(new AST::Description(module_list));

	AST::Source::Ptr *psrc = new AST::Source::Ptr(new AST::Source(desc));
	AST::Source::Ptr &src = *psrc;

	AST::Source::Ptr src_clone = AST::cast_to<AST::Source>(src->clone());

	ASSERT_TRUE(*src == *src);
	ASSERT_FALSE(*src != *src);

	ASSERT_TRUE(*src_clone == *src);
	ASSERT_FALSE(*src_clone != *src);

	ASSERT_TRUE(src_clone->get_description().get() != src->get_description().get());
	ASSERT_TRUE(src_clone->get_description().get() != src->get_description().get());

	delete psrc;

	ASSERT_TRUE(*src_clone == *src_clone);
	ASSERT_FALSE(*src_clone != *src_clone);

}


TEST(ASTTest, CloneList) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("ASTTest.CloneList.log");
	Logger::add_stdout_sink();

	AST::IntConst::Ptr width_msb = std::make_shared<AST::IntConst>("filename.v", 3);
	width_msb->set_value("7");
	AST::IntConst::Ptr width_lsb = std::make_shared<AST::IntConst>("filename.v", 3);
	width_lsb->set_value("0");

	AST::Width::Ptr width = std::make_shared<AST::Width>(AST::to_node(width_msb),
	                                                          AST::to_node(width_lsb),
	                                                          "filename.v", 3);
	AST::Width::ListPtr widths = std::make_shared<AST::Width::List>();
	widths->push_back(width);

	AST::IntConst::Ptr length_msb = std::make_shared<AST::IntConst>("filename.v", 3);
	length_msb->set_value("1023");
	AST::IntConst::Ptr length_lsb = std::make_shared<AST::IntConst>("filename.v", 3);
	length_lsb->set_value("0");

	AST::Length::Ptr length = std::make_shared<AST::Length>(AST::to_node(length_msb),
	                                                              AST::to_node(length_lsb),
	                                                              "filename.v", 3);
	AST::Length::ListPtr lengths = std::make_shared<AST::Length::List>();
	lengths->push_back(length);


	AST::Wire::Ptr wire0 = std::make_shared<AST::Wire>(widths,
	                                                   AST::DelayStatement::Ptr(),
	                                                   AST::DelayStatement::Ptr(),
	                                                   lengths, AST::Rvalue::Ptr(),
	                                                   false, "wire0", "filename", 3);

	AST::Wire::Ptr wire1 = std::make_shared<AST::Wire>(AST::Width::ListPtr(),
	                                                   AST::DelayStatement::Ptr(),
	                                                   AST::DelayStatement::Ptr(),
	                                                   AST::Length::ListPtr(),
	                                                   AST::Rvalue::Ptr(),
	                                                   false, "wire1", "filename", 3);

	AST::Wire::Ptr wire2 = std::make_shared<AST::Wire>(AST::Width::ListPtr(),
	                                                   AST::DelayStatement::Ptr(),
	                                                   AST::DelayStatement::Ptr(),
	                                                   AST::Length::ListPtr(),
	                                                   AST::Rvalue::Ptr(),
	                                                   false, "wire2", "filename", 3);

	AST::Wire::ListPtr wire_list = std::make_shared<AST::Wire::List>();

	wire_list->push_back(wire0);
	wire_list->push_back(wire1);
	wire_list->push_back(wire2);

	// We clone the list and we expect that all list elements with the
	// cloned version are the same
	AST::Wire::ListPtr wire_list_cloned = AST::Wire::clone_list(wire_list);

	AST::Wire::List::const_iterator it = wire_list->begin();
	AST::Wire::List::const_iterator itc = wire_list_cloned->begin();

	for(int i=0; (it != wire_list->end()) && (itc != wire_list_cloned->end()); ++it, ++itc, ++i) {
		AST::Wire::Ptr v = *it;
		AST::Wire::Ptr vc = *itc;
		ASSERT_TRUE(v->is_equal(*vc));
	}

	ASSERT_FALSE((it != wire_list->end()) || (itc != wire_list_cloned->end()));

	// We change a value and we expect a difference
	wire2->set_name("wireX");

	bool broken=false;

	it = wire_list->begin();
	itc = wire_list_cloned->begin();

	for(; (it != wire_list->end()) && (itc != wire_list_cloned->end()); ++it, ++itc) {
		AST::Wire::Ptr v = *it;
		AST::Wire::Ptr vc = *itc;
		if(v->is_not_equal(*vc)) {
			broken = true;
		}
	}

	ASSERT_TRUE(broken);
	ASSERT_FALSE((it != wire_list->end()) || (itc != wire_list_cloned->end()));

	// We reset the initial version, everything is deleted. So we are
	// checking that the cloned version is still alive. ie if
	// everything was copied and not just referenced.
	wire_list.reset();
	ASSERT_FALSE(wire_list);

	for(const AST::Wire::Ptr wc: *wire_list_cloned) {
		if(wc->is_not_equal(*wc)) {
			broken = true;
		}
	}


}


TEST(ASTTest, UpdateChildren) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("ASTTest.UpdateChildren.log");
	Logger::add_stdout_sink();


	AST::Parameter::ListPtr params1 = std::make_shared<AST::Parameter::List>();
	params1->push_back(std::make_shared<AST::Parameter>());

	AST::Node::ListPtr ports1 = std::make_shared<AST::Node::List>();
	ports1->push_back(std::make_shared<AST::Port>());

	AST::Node::ListPtr items1(new AST::Node::List);


	AST::Parameter::ListPtr params2 = std::make_shared<AST::Parameter::List>();
	params2->push_back(std::make_shared<AST::Parameter>());

	AST::Node::ListPtr ports2 = std::make_shared<AST::Node::List>();
	ports2->push_back(std::make_shared<AST::Port>());

	AST::Node::ListPtr items2(new AST::Node::List);


	AST::Node::Ptr mod1(new AST::Module(params1, ports1, items1,
	                                    std::string("module1"), AST::Module::Default_nettypeEnum::WIRE));
	AST::Node::Ptr mod2(new AST::Module(params2, ports2, items2,
	                                    std::string("module2"), AST::Module::Default_nettypeEnum::WIRE));

	AST::Parameter::ListPtr params_null = nullptr;
	AST::Node::Ptr mod3(new AST::Module(params_null, ports2, items2,
	                                    std::string("module2"), AST::Module::Default_nettypeEnum::WIRE));

	AST::Node::ListPtr module_list(new AST::Node::List);
	module_list->push_back(mod1);
	module_list->push_back(mod2);

	AST::Description::Ptr desc(new AST::Description(module_list));

	ASSERT_EQ(mod1->get_children()->size(), (unsigned) 2);
	ASSERT_EQ(mod2->get_children()->size(), (unsigned) 2);
	ASSERT_EQ(mod3->get_children()->size(), (unsigned) 1);
	ASSERT_EQ(desc->get_children()->size(), (unsigned) 2);

	AST::cast_to<AST::Module>(mod1)->set_params(nullptr);
	AST::cast_to<AST::Module>(mod2)->set_params(nullptr);
	AST::cast_to<AST::Module>(mod2)->set_ports(nullptr);

	desc->set_definitions(AST::Node::ListPtr(nullptr));

	ASSERT_EQ(mod1->get_children()->size(), (unsigned) 1);
	ASSERT_EQ(mod2->get_children()->size(), (unsigned) 0);
	ASSERT_EQ(desc->get_children()->size(), (unsigned) 0);
}
