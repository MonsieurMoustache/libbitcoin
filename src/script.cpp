#include <bitcoin/script.hpp>

#include <bitcoin/util/logger.hpp>

namespace libbitcoin {

void script::push_operation(operation oper)
{
    operations_.push_back(oper);
}

std::string script::string_repr()
{
    std::ostringstream ss;
    for (auto it = operations_.cbegin(); it != operations_.cend(); ++it)
    {
        const operation op = *it;
        if (op.data.size() == 0)
            ss << opcode_to_string(op.code) << " ";
        else
        {
            ss << "[ ";
            for (auto dit = op.data.cbegin(); dit != op.data.cend(); ++dit)
                ss << std::hex << int(*dit) << " ";
            ss << "] ";
        }
    }
    return ss.str();
}

std::string opcode_to_string(opcode code)
{
    switch (code)
    {
        case opcode::special:
            return "special";
        case opcode::pushdata1:
            return "pushdata1";
        case opcode::pushdata2:
            return "pushdata2";
        case opcode::pushdata4:
            return "pushdata4";
        case opcode::dup:
            return "dup";
        case opcode::hash160:
            return "hash160";
        case opcode::equalverify:
            return "equalverify";
        case opcode::checksig:
            return "checksig";
        default:
        {
            std::ostringstream ss;
            ss << "<none " << static_cast<int>(code) << ">";
            return ss.str();
        }
    }
}

size_t number_of_bytes_from_opcode(opcode code, byte raw_byte)
{
    if (code == opcode::special)
        return raw_byte;
    else if (code == opcode::pushdata1 || code == opcode::pushdata2 
            || code == opcode::pushdata4)
        return static_cast<size_t>(code);
    else
        return 0;
}

script parse_script(data_chunk raw_script)
{
    script script_object;
    for (auto it = raw_script.cbegin(); it != raw_script.cend(); ++it)
    {
        byte raw_byte = *it;
        operation op;
        op.code = static_cast<opcode>(*it);
        if (1 <= raw_byte && raw_byte <= 75)
            op.code = opcode::special;
        size_t read_n_bytes = number_of_bytes_from_opcode(op.code, raw_byte);
        logger(LOG_DEBUG) << "Parsed: " << opcode_to_string(op.code);
        if (read_n_bytes > 0)
            logger(LOG_DEBUG) << "Reading: " << read_n_bytes << " bytes...";

        for (size_t byte_count = 0; byte_count < read_n_bytes; ++byte_count)
        {
            if (it == raw_script.cend())
            {
                logger(LOG_ERROR) << "Premature end of script.";
                break;
            }

            ++it;
            op.data.push_back(*it);
        }
        script_object.push_operation(op);
    }
    return script_object;
}

} // libbitcoin

