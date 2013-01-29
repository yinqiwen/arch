/**
 * 
 */
package gen;

/**
 * @author qiyingwang
 * 
 */
public class CodeGen
{

	private static StringBuffer genEncode(int count, StringBuffer buffer)
	{
		buffer.append("template<");
		for (int i = 0; i < count; i++)
		{
			buffer.append("typename A").append(i);
			if (i != count - 1)
			{
				buffer.append(",");
			}
		}
		buffer.append(">\n");
		buffer.append("inline bool encode_arg(Buffer& buf, ");
		for (int i = 0; i < count; i++)
		{
			buffer.append("A").append(i).append("& a").append(i);
			if (i != count - 1)
			{
				buffer.append(", ");
			}
		}
		buffer.append(")\n");
		buffer.append("{\n");
		buffer.append("return encode_arg(buf,");
		if (count > 1)
		{
			int i = 0;
			for (i = 0; i < count - 1; i++)
			{
				buffer.append("a").append(i);
				if (i != count - 2)
				{
					buffer.append(", ");
				}
			}
			buffer.append(") &&  encode_arg(buf, a").append(i).append(");\n");
		}
		else
		{
			buffer.append("a0);\n");
		}
		buffer.append("}\n");
		return buffer;
	}

	private static StringBuffer genDecode(int count, StringBuffer buffer)
	{
		buffer.append("template<");
		for (int i = 0; i < count; i++)
		{
			buffer.append("typename A").append(i);
			if (i != count - 1)
			{
				buffer.append(",");
			}
		}
		buffer.append(">\n");
		buffer.append("inline bool decode_arg(Buffer& buf, ");
		for (int i = 0; i < count; i++)
		{
			buffer.append("A").append(i).append("& a").append(i);
			if (i != count - 1)
			{
				buffer.append(", ");
			}
		}
		buffer.append(")\n");
		buffer.append("{\n");
		buffer.append("return decode_arg(buf,");
		if (count > 1)
		{
			int i = 0;
			for (i = 0; i < count - 1; i++)
			{
				buffer.append("a").append(i);
				if (i != count - 2)
				{
					buffer.append(", ");
				}
			}
			buffer.append(") &&  decode_arg(buf, a").append(i).append(");\n");
		}
		else
		{
			buffer.append("a0);\n");
		}
		buffer.append("}\n");
		return buffer;
	}

	private static StringBuffer genFixEncode(int count, StringBuffer buffer)
	{
		buffer.append("template<");
		for (int i = 0; i < count; i++)
		{
			buffer.append("typename A").append(i);
			if (i != count - 1)
			{
				buffer.append(",");
			}
		}
		buffer.append(">\n");
		buffer.append("inline bool fix_encode_arg(Buffer& buf, ");
		for (int i = 0; i < count; i++)
		{
			buffer.append("A").append(i).append("& a").append(i);
			if (i != count - 1)
			{
				buffer.append(", ");
			}
		}
		buffer.append(")\n");
		buffer.append("{\n");
		buffer.append("return fix_encode_arg(buf,");
		if (count > 1)
		{
			int i = 0;
			for (i = 0; i < count - 1; i++)
			{
				buffer.append("a").append(i);
				if (i != count - 2)
				{
					buffer.append(", ");
				}
			}
			buffer.append(") &&  fix_encode_arg(buf, a").append(i)
			        .append(");\n");
		}
		else
		{
			buffer.append("a0);\n");
		}
		buffer.append("}\n");
		return buffer;
	}

	private static StringBuffer genFixDecode(int count, StringBuffer buffer)
	{
		buffer.append("template<");
		for (int i = 0; i < count; i++)
		{
			buffer.append("typename A").append(i);
			if (i != count - 1)
			{
				buffer.append(",");
			}
		}
		buffer.append(">\n");
		buffer.append("inline bool fix_decode_arg(Buffer& buf, ");
		for (int i = 0; i < count; i++)
		{
			buffer.append("A").append(i).append("& a").append(i);
			if (i != count - 1)
			{
				buffer.append(", ");
			}
		}
		buffer.append(")\n");
		buffer.append("{\n");
		buffer.append("return fix_decode_arg(buf,");
		if (count > 1)
		{
			int i = 0;
			for (i = 0; i < count - 1; i++)
			{
				buffer.append("a").append(i);
				if (i != count - 2)
				{
					buffer.append(", ");
				}
			}
			buffer.append(") &&  fix_decode_arg(buf, a").append(i)
			        .append(");\n");
		}
		else
		{
			buffer.append("a0);\n");
		}
		buffer.append("}\n");
		return buffer;
	}

	/**
	 * @param args
	 */
	public static void main(String[] args)
	{
		// TODO Auto-generated method stub
		StringBuffer buf = new StringBuffer();
		for (int i = 2; i < 65; i++)
		{
			genDecode(i, buf);
		}

		StringBuffer buf2 = new StringBuffer();
		for (int i = 2; i < 65; i++)
		{
			genEncode(i, buf2);
		}
		StringBuffer buf3 = new StringBuffer();
		for (int i = 2; i < 65; i++)
		{
			genFixDecode(i, buf3);
		}

		StringBuffer buf4 = new StringBuffer();
		for (int i = 2; i < 65; i++)
		{
			genFixEncode(i, buf4);
		}
		String header_macro_bagin = "#ifndef STRUCT_CODE_TEMPLATES_HPP_\r\n"
		        + "#define STRUCT_CODE_TEMPLATES_HPP_\r\n\r\n";
		String header_macro_end = "#endif /* STRUCT_CODE_TEMPLATES_HPP_ */";
		String namespace_begin = "namespace arch{namespace buffer{\r\n";
		String namespace_end = "}}\r\n";
		System.out.println(header_macro_bagin);
		System.out.println(namespace_begin);
		System.out.println(buf);
		System.out.println(buf2);
		System.out.println(buf3);
		System.out.println(buf4);
		System.out.println(namespace_end);
		System.out.println(header_macro_end);
	}

}
