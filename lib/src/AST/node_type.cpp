#include <veriparse/AST/node_type.hpp>
#include <vector>


namespace Veriparse {
	namespace AST {

		const char* NodeTypeToString(NodeType node_type) {
			switch (node_type) {
			case NodeType::Node: return "Node";
			case NodeType::Source: return "Source";
			case NodeType::Description: return "Description";
			case NodeType::Pragma: return "Pragma";
			case NodeType::Module: return "Module";
			case NodeType::Port: return "Port";
			case NodeType::Width: return "Width";
			case NodeType::Length: return "Length";
			case NodeType::Identifier: return "Identifier";
			case NodeType::Constant: return "Constant";
			case NodeType::StringConst: return "StringConst";
			case NodeType::IntConst: return "IntConst";
			case NodeType::IntConstN: return "IntConstN";
			case NodeType::FloatConst: return "FloatConst";
			case NodeType::IODir: return "IODir";
			case NodeType::Input: return "Input";
			case NodeType::Output: return "Output";
			case NodeType::Inout: return "Inout";
			case NodeType::VariableBase: return "VariableBase";
			case NodeType::Genvar: return "Genvar";
			case NodeType::Variable: return "Variable";
			case NodeType::Net: return "Net";
			case NodeType::Integer: return "Integer";
			case NodeType::Real: return "Real";
			case NodeType::Tri: return "Tri";
			case NodeType::Wire: return "Wire";
			case NodeType::Supply0: return "Supply0";
			case NodeType::Supply1: return "Supply1";
			case NodeType::Reg: return "Reg";
			case NodeType::Ioport: return "Ioport";
			case NodeType::Parameter: return "Parameter";
			case NodeType::Localparam: return "Localparam";
			case NodeType::Concat: return "Concat";
			case NodeType::Lconcat: return "Lconcat";
			case NodeType::Repeat: return "Repeat";
			case NodeType::Indirect: return "Indirect";
			case NodeType::Partselect: return "Partselect";
			case NodeType::PartselectIndexed: return "PartselectIndexed";
			case NodeType::PartselectPlusIndexed: return "PartselectPlusIndexed";
			case NodeType::PartselectMinusIndexed: return "PartselectMinusIndexed";
			case NodeType::Pointer: return "Pointer";
			case NodeType::Lvalue: return "Lvalue";
			case NodeType::Rvalue: return "Rvalue";
			case NodeType::UnaryOperator: return "UnaryOperator";
			case NodeType::Uplus: return "Uplus";
			case NodeType::Uminus: return "Uminus";
			case NodeType::Ulnot: return "Ulnot";
			case NodeType::Unot: return "Unot";
			case NodeType::Uand: return "Uand";
			case NodeType::Unand: return "Unand";
			case NodeType::Uor: return "Uor";
			case NodeType::Unor: return "Unor";
			case NodeType::Uxor: return "Uxor";
			case NodeType::Uxnor: return "Uxnor";
			case NodeType::Operator: return "Operator";
			case NodeType::Power: return "Power";
			case NodeType::Times: return "Times";
			case NodeType::Divide: return "Divide";
			case NodeType::Mod: return "Mod";
			case NodeType::Plus: return "Plus";
			case NodeType::Minus: return "Minus";
			case NodeType::Sll: return "Sll";
			case NodeType::Srl: return "Srl";
			case NodeType::Sra: return "Sra";
			case NodeType::LessThan: return "LessThan";
			case NodeType::GreaterThan: return "GreaterThan";
			case NodeType::LessEq: return "LessEq";
			case NodeType::GreaterEq: return "GreaterEq";
			case NodeType::Eq: return "Eq";
			case NodeType::NotEq: return "NotEq";
			case NodeType::Eql: return "Eql";
			case NodeType::NotEql: return "NotEql";
			case NodeType::And: return "And";
			case NodeType::Xor: return "Xor";
			case NodeType::Xnor: return "Xnor";
			case NodeType::Or: return "Or";
			case NodeType::Land: return "Land";
			case NodeType::Lor: return "Lor";
			case NodeType::Cond: return "Cond";
			case NodeType::Always: return "Always";
			case NodeType::Senslist: return "Senslist";
			case NodeType::Sens: return "Sens";
			case NodeType::Defparamlist: return "Defparamlist";
			case NodeType::Defparam: return "Defparam";
			case NodeType::Assign: return "Assign";
			case NodeType::BlockingSubstitution: return "BlockingSubstitution";
			case NodeType::NonblockingSubstitution: return "NonblockingSubstitution";
			case NodeType::IfStatement: return "IfStatement";
			case NodeType::RepeatStatement: return "RepeatStatement";
			case NodeType::ForStatement: return "ForStatement";
			case NodeType::WhileStatement: return "WhileStatement";
			case NodeType::CaseStatement: return "CaseStatement";
			case NodeType::CasexStatement: return "CasexStatement";
			case NodeType::CasezStatement: return "CasezStatement";
			case NodeType::Case: return "Case";
			case NodeType::Block: return "Block";
			case NodeType::Initial: return "Initial";
			case NodeType::EventStatement: return "EventStatement";
			case NodeType::WaitStatement: return "WaitStatement";
			case NodeType::ForeverStatement: return "ForeverStatement";
			case NodeType::DelayStatement: return "DelayStatement";
			case NodeType::Instancelist: return "Instancelist";
			case NodeType::Instance: return "Instance";
			case NodeType::ParamArg: return "ParamArg";
			case NodeType::PortArg: return "PortArg";
			case NodeType::Function: return "Function";
			case NodeType::FunctionCall: return "FunctionCall";
			case NodeType::Task: return "Task";
			case NodeType::TaskCall: return "TaskCall";
			case NodeType::GenerateStatement: return "GenerateStatement";
			case NodeType::SystemCall: return "SystemCall";
			case NodeType::IdentifierScopeLabel: return "IdentifierScopeLabel";
			case NodeType::IdentifierScope: return "IdentifierScope";
			case NodeType::Disable: return "Disable";
			case NodeType::ParallelBlock: return "ParallelBlock";
			case NodeType::SingleStatement: return "SingleStatement";
			default: return "Unknown NodeType";
			}
		}

		std::ostream & operator<<(std::ostream &os, const NodeType node_type)
		{
			os << NodeTypeToString(node_type);
			return os;
		}

		std::ostream & operator<<(std::ostream &os, const std::vector<NodeType> &node_categories)
		{
			os << "[";
			for(const NodeType &cat: node_categories) {
				os << NodeTypeToString(cat);
				if(cat != node_categories.back())
					os << ", ";
			}
			os << "]";
			return os;
		}

	}
}