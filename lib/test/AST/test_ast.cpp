// SPDX-License-Identifier: LGPL-3.0-or-later
// Copyright (C) 2013-2026 Christophe Clienti
#include <gtest/gtest.h>
#include <veriparse/AST/nodes.hpp>
#include <veriparse/logger/logger.hpp>
#include <iostream>

using namespace Veriparse;

TEST(ASTTest, Clone)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("ASTTest.Clone.log");
    Logger::add_stderr_sink();

    AST::Declaration::ListPtr params1;
    AST::Port::ListPtr ports1;
    AST::Node::ListPtr items1(new AST::Node::List);

    AST::Declaration::ListPtr params2;
    AST::Port::ListPtr ports2;
    AST::Node::ListPtr items2(new AST::Node::List);

    AST::Node::Ptr mod1(new AST::Module(params1, ports1, items1, std::string("module1"),
                                        AST::Module::LifetimeEnum::NONE,
                                        AST::Module::Default_nettypeEnum::WIRE));
    AST::Node::Ptr mod2(new AST::Module(params2, ports2, items2, std::string("module2"),
                                        AST::Module::LifetimeEnum::NONE,
                                        AST::Module::Default_nettypeEnum::WIRE));

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

TEST(ASTTest, CloneList)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("ASTTest.CloneList.log");
    Logger::add_stderr_sink();

    // Packed dimension [7:0], carried by the net data type.
    AST::IntConst::Ptr packed_left = std::make_shared<AST::IntConst>("filename.v", 3);
    packed_left->set_value("7");
    AST::IntConst::Ptr packed_right = std::make_shared<AST::IntConst>("filename.v", 3);
    packed_right->set_value("0");
    AST::Dimension::ListPtr packed_dims = std::make_shared<AST::Dimension::List>();
    packed_dims->push_back(std::make_shared<AST::RangeDim>(
        AST::to_node(packed_left), AST::to_node(packed_right), "filename.v", 3));

    AST::ImplicitType::Ptr type = std::make_shared<AST::ImplicitType>("filename.v", 3);
    type->set_packed_dims(packed_dims);

    // Unpacked dimension [1023:0], carried by the declaration.
    AST::IntConst::Ptr unpacked_left = std::make_shared<AST::IntConst>("filename.v", 3);
    unpacked_left->set_value("1023");
    AST::IntConst::Ptr unpacked_right = std::make_shared<AST::IntConst>("filename.v", 3);
    unpacked_right->set_value("0");
    AST::Dimension::ListPtr unpacked_dims = std::make_shared<AST::Dimension::List>();
    unpacked_dims->push_back(std::make_shared<AST::RangeDim>(
        AST::to_node(unpacked_left), AST::to_node(unpacked_right), "filename.v", 3));

    AST::WireNet::Ptr wire0 = std::make_shared<AST::WireNet>(
        unpacked_dims, AST::Rvalue::Ptr(), AST::Strength::Ptr(), AST::DelayStatement::Ptr(),
        AST::DelayStatement::Ptr(), type, false, false, "wire0", "filename", 3);

    AST::WireNet::Ptr wire1 = std::make_shared<AST::WireNet>("filename", 3);
    wire1->set_name("wire1");

    AST::WireNet::Ptr wire2 = std::make_shared<AST::WireNet>("filename", 3);
    wire2->set_name("wire2");

    AST::WireNet::ListPtr wire_list = std::make_shared<AST::WireNet::List>();

    wire_list->push_back(wire0);
    wire_list->push_back(wire1);
    wire_list->push_back(wire2);

    // We clone the list and we expect that all list elements with the
    // cloned version are the same
    AST::WireNet::ListPtr wire_list_cloned = AST::WireNet::clone_list(wire_list);

    AST::WireNet::List::const_iterator it = wire_list->begin();
    AST::WireNet::List::const_iterator itc = wire_list_cloned->begin();

    for(; (it != wire_list->end()) && (itc != wire_list_cloned->end()); ++it, ++itc) {
        AST::WireNet::Ptr v = *it;
        AST::WireNet::Ptr vc = *itc;
        ASSERT_TRUE(v->is_equal(*vc));
    }

    ASSERT_FALSE((it != wire_list->end()) || (itc != wire_list_cloned->end()));

    // We change a value and we expect a difference
    wire2->set_name("wireX");

    bool broken = false;

    it = wire_list->begin();
    itc = wire_list_cloned->begin();

    for(; (it != wire_list->end()) && (itc != wire_list_cloned->end()); ++it, ++itc) {
        AST::WireNet::Ptr v = *it;
        AST::WireNet::Ptr vc = *itc;
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

    for(const AST::WireNet::Ptr &wc : *wire_list_cloned) {
        if(wc->is_not_equal(*wc)) {
            broken = true;
        }
    }
}

TEST(ASTTest, UpdateChildren)
{
    Logger::remove_all_sinks();
    Logger::add_text_sink("ASTTest.UpdateChildren.log");
    Logger::add_stderr_sink();

    AST::Declaration::ListPtr params1 = std::make_shared<AST::Declaration::List>();
    params1->push_back(std::make_shared<AST::Param>());

    AST::Port::ListPtr ports1 = std::make_shared<AST::Port::List>();
    ports1->push_back(std::make_shared<AST::Port>());

    AST::Node::ListPtr items1(new AST::Node::List);

    AST::Declaration::ListPtr params2 = std::make_shared<AST::Declaration::List>();
    params2->push_back(std::make_shared<AST::Param>());

    AST::Port::ListPtr ports2 = std::make_shared<AST::Port::List>();
    ports2->push_back(std::make_shared<AST::Port>());

    AST::Node::ListPtr items2(new AST::Node::List);

    AST::Node::Ptr mod1(new AST::Module(params1, ports1, items1, std::string("module1"),
                                        AST::Module::LifetimeEnum::NONE,
                                        AST::Module::Default_nettypeEnum::WIRE));
    AST::Node::Ptr mod2(new AST::Module(params2, ports2, items2, std::string("module2"),
                                        AST::Module::LifetimeEnum::NONE,
                                        AST::Module::Default_nettypeEnum::WIRE));

    AST::Declaration::ListPtr params_null = nullptr;
    AST::Node::Ptr mod3(new AST::Module(params_null, ports2, items2, std::string("module2"),
                                        AST::Module::LifetimeEnum::NONE,
                                        AST::Module::Default_nettypeEnum::WIRE));

    AST::Node::ListPtr module_list(new AST::Node::List);
    module_list->push_back(mod1);
    module_list->push_back(mod2);

    AST::Description::Ptr desc(new AST::Description(module_list));

    ASSERT_EQ(mod1->get_children()->size(), (unsigned)2);
    ASSERT_EQ(mod2->get_children()->size(), (unsigned)2);
    ASSERT_EQ(mod3->get_children()->size(), (unsigned)1);
    ASSERT_EQ(desc->get_children()->size(), (unsigned)2);

    AST::cast_to<AST::Module>(mod1)->set_params(nullptr);
    AST::cast_to<AST::Module>(mod2)->set_params(nullptr);
    AST::cast_to<AST::Module>(mod2)->set_ports(nullptr);

    desc->set_definitions(AST::Node::ListPtr(nullptr));

    ASSERT_EQ(mod1->get_children()->size(), (unsigned)1);
    ASSERT_EQ(mod2->get_children()->size(), (unsigned)0);
    ASSERT_EQ(desc->get_children()->size(), (unsigned)0);
}
