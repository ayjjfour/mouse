local gOpt = {
	["optional"] = 0,
	["required"] = 1,
	["repeated"] = 2,
	["packed"  ] = 3,
}
local gType = {
	["uint32"  ] = 1,
	[1] = "Uint",
	["uint64"  ] = 2,
	[2] = "Int64",
	["int32"   ] = 3,
	[3] = "Int",
	["int64"   ] = 4,
	[4] = "Int64",
	["string"  ] = 5,
	[5] = "String",
	["bool"    ] = 6,
	[6] = "Bool",
	["bytes"   ] = 7,
	[7] = "Ptr",
	["float"   ] = 8,
	[8] = "Float",
	["double"  ] = 9,
	[9] = "Float",
	["sint32"  ] = 10,
	[10] = "Int",
	["sint64"  ] = 11,
	[11] = "Int64",
	["fixed32" ] = 12,
	[12] = "Uint",
	["fixed64" ] = 13,
	[13] = "Int64",
	["sfixed32"] = 14,
	[14] = "Int32",
	["sfixed64"] = 15,
	[15] = "Int64",
}

local es = {};
function LoadFile( f )--加载文件并剔除无用字符串
	local s = f:read("*a")
	s = string.gsub(s, "//[^\r\n]*", "");
	s = string.gsub(s, "pb_common.", "");
	s = string.gsub(s, "bytes result_info", "GameResultInfo result_info");
	s = string.gsub(s, "bytes extend", "DeskExtendInfo extend");
	s = string.gsub(s, "bytes ready_extend", "DeskReadyExtendInfo extend");
	
	f:close();
	for n, m in string.gmatch(s, "%s*enum ([%w_]+)%s*{(.-)}") do
		table.insert(es, n);
	end
	for i,n in ipairs(es) do
		s = string.gsub(s, "optional "..n.." ", "optional uint32 ");
	end
	
	s = string.gsub(s, "enum [%w_]+%s*{.-}", "");
	return s;
end

function LoadFileWithEnum( f )--加载文件并剔除无用字符串
	local s = f:read("*a")
	s = string.gsub(s, "//[^\r\n]*", "");
	s = string.gsub(s, "pb_common.", "");
	s = string.gsub(s, "bytes result_info", "GameResultInfo result_info");
	s = string.gsub(s, "bytes extend", "DeskExtendInfo extend");
	s = string.gsub(s, "bytes ready_extend", "DeskReadyExtendInfo extend");
	
	f:close();
	for n, m in string.gmatch(s, "%s*enum ([%w_]+)%s*{(.-)}") do
		table.insert(es, n);
	end
	for i,n in ipairs(es) do
		s = string.gsub(s, "optional "..n.." ", "optional uint32 ");
	end
	
	return s;
end



function tagCmdToXml( filename )--生成消息枚举
	local f = io.open(filename, "r");
	local s = LoadFileWithEnum(f);
	local out = "";
	for n, m in string.gmatch(s, "%s*enum ([%w_]+)%s*{(.-)}") do
		local idx = 0;
		out = out.."<Enum name=\""..n.."\">\n";
		for name, id in string.gmatch(m, "([%w_]+)(.-),") do
			local v = string.gmatch(id, "%s*=%s*(%d+)")();
			if v then idx = v; else idx = idx + 1 end
			local flag = 1;
			--local ss = "";
		--	for i=1,string.len(name) do
			--	local c = string.sub(name,i,i);
			--	if c == "_" then
			--		flag = 1;
			--	elseif flag == 1 then
			--		ss = ss..string.upper(c);
			--		flag = 0;
			--	else
			--		ss = ss..string.lower(c);
			--	end
		--	end
			local ss=name;
			out = out.."\t".."<Message Name=\""..ss.."\" Tag=\""..idx.."\"/>\n";
		end
		out = out.."</Enum>\n";
	end
	if string.len(out) > 0 then
		local emf = io.open("GameMessageTags.xml", "w");
		emf:write(out);
		emf:close();
	end
end

function tagCmdToLua( filename )--生成消息枚举
	local f = io.open(filename, "r");
	local s = LoadFileWithEnum(f);
	local out = "GameMessageTags = \n{\n";
	for n, m in string.gmatch(s, "%s*enum ([%w_]+)%s*{(.-)}") do
		local idx = 0;
		
		for name, id in string.gmatch(m, "([%w_]+)(.-),") do
			local v = string.gmatch(id, "%s*=%s*(%d+)")();
			if v then idx = v; else idx = idx + 1 end
			local flag = 1;
			--local ss = "";
		--	for i=1,string.len(name) do
			--	local c = string.sub(name,i,i);
			--	if c == "_" then
			--		flag = 1;
			--	elseif flag == 1 then
			--		ss = ss..string.upper(c);
			--		flag = 0;
			--	else
			--		ss = ss..string.lower(c);
			--	end
		--	end
			local ss=name;
			out = out.."\t"..ss.."=\""..ss.."\",\n";
		end
		out = out.."}\n";
	end
	if string.len(out) > 0 then
		local emf = io.open("GameMessageTags.lua", "w");
		emf:write(out);
		emf:close();
	end
end

function tagCmdToJS( filename )--生成消息枚举
	local f = io.open(filename, "r");
	local s = LoadFileWithEnum(f);
	local out = "var MessageTags = \n{\n";
	for n, m in string.gmatch(s, "%s*enum ([%w_]+)%s*{(.-)}") do
		local idx = 0;
		
		for name, id in string.gmatch(m, "([%w_]+)(.-),") do
			local v = string.gmatch(id, "%s*=%s*(%d+)")();
			if v then idx = v; else idx = idx + 1 end
			local flag = 1;
			--local ss = "";
		--	for i=1,string.len(name) do
			--	local c = string.sub(name,i,i);
			--	if c == "_" then
			--		flag = 1;
			--	elseif flag == 1 then
			--		ss = ss..string.upper(c);
			--		flag = 0;
			--	else
			--		ss = ss..string.lower(c);
			--	end
		--	end
			local ss=name;
			out = out.."\t"..ss..":\""..ss.."\",\n";
		end
		out = out.."}\n";
	end
	if string.len(out) > 0 then
		local emf = io.open("GameMessageTags.js", "w");
		emf:write(out);
		emf:close();
	end
end

local proto = {};
function SetProto( s )
	for n, m in string.gmatch(s, "%s*message%s+([%w_]+)%s*{(.-)}") do
		local item = {};
		item = {};
		for opt,tp,name,id,def in string.gmatch(m, "([%w_]+)%s+([%w_%.]+)%s+([%w_]+)%s*=%s*(%d+)%s*%[?(.-)%]?;") do
			local p = string.gmatch(def, "(packed)%s*=%s*true")();
			local v = string.gmatch(def, "default%s*=%s*(%d+)")();
			if not v then v = ""; end
			opt = p and gOpt[p] or gOpt[opt]
			tp = string.gsub(tp, "[%w_]+%.", "");
			tp = gType[tp] or tp;
			item[id] = {name,opt,tp,v}
		end
		proto[n] = item;
	end
end

function ParseProtoFile( filename )
	local f = assert(io.open(filename, "r"));
	local s = LoadFile(f);
	SetProto(s);
end

function ProtoToJs()
	local f = io.open("GameMessage.json", "w");
	local flag = "";
	f:write("{");
	for name, tb in pairs(proto) do
		f:write(flag..""..name..":\"{");
		flag = ",\n"
		local flag2 = "";
		for id, v in pairs(tb) do
			f:write(flag2..""..id..":["..v[1]..","..v[2]..","..v[3]..","..v[4].."]");
			flag2 = ","
		end
		f:write("}\"");
	end
	f:write("}\n");
	f:close();
end

function ReqWriteStruct(f, name, tp)
	local struct = proto[tp];
	if struct then
		f:write("\tlocal "..name.." = LuaVar(\""..tp.."\");\n");
		for id, v in pairs(struct) do
			local b = ReqWriteStruct(f, v[1], v[3]);
			if b then
				f:write("\t"..name.."."..v[1].." = "..v[1]..".this;\n");
			else
				if v[2] > 1 then
					f:write("\t"..name.."."..v[1].."[0].Set"..gType[v[3]].."()"..";\n");
				else
					f:write("\t"..name.."."..v[1]..".Set"..gType[v[3]].."()"..";\n");
				end
			end
		end
		return true;
	end
	return false;
end

function ReqToLua()
	local f = io.open("req.lua", "w");
	for name, tb in pairs(proto) do
		if string.sub(name,1,6) == "MsgReq" then
			f:write("function on"..name.."(aParam, aNode)\n");
			ReqWriteStruct(f, "var", name);
			f:write("\tReqSend(var.this, \"\");\nend\n");
		end
	end
	f:close();
end

function VarWriteStruct(f, name, tp, parent)
	local struct = proto[tp];
	if struct then
		if parent then
			f:write("\tlocal a"..tp.." = a"..parent..":D(\""..name.."\", gNullVar);\n");
			f:write("\tlocal "..name.." = {};\n");
		end
		for id, v in pairs(struct) do
			local b = VarWriteStruct(f, v[1], v[3], tp);
			if b then
				f:write("\t"..name.."."..v[1].." = "..v[1]..";\n");
			else
				f:write("\t"..name.."."..v[1].." = a"..tp..":T(\""..v[1].."\");\n");
			end
		end
		return true;
	end
	return false;
end

function VarToLua()
	local f = io.open("var.lua", "w");
	for name, tb in pairs(proto) do
		f:write("function get"..name.."( a"..name.." )\n");
		f:write("\tlocal t"..name.." = {};\n");
		VarWriteStruct(f, "t"..name, name, nil);
		f:write("\treturn t"..name..";\nend\n");
	end
	f:close();
end
function GetName(name)
	if gNames[name] then
		gNames[name] = gNames[name] + 1; 
		name = name.."_"..gNames[name];
	else
		gNames[name] = 0;
	end
	return name;
end
function RspWriteStruct(f, name, tp, parent, opt)
	local struct = proto[tp];
	if struct then
		if parent then
			local v1 = GetName(name);
			if opt > 1 then
				f:write("\tlocal "..v1.." = "..parent.."."..name..";--Is list\n");
			else
				f:write("\tlocal "..v1.." = "..parent.."."..name..";\n");
			end
			name = v1;
		end
		for id, v in pairs(struct) do
			local b = RspWriteStruct(f, v[1], v[3], name, v[2]);
			if not b then
				local v1 = GetName(v[1]);
				if opt > 1 then
					if v[2] > 1 then
						f:write("--\tlocal "..v1.." = "..name.."[0]."..v[1].."[0]."..gType[v[3]].."();\r");
					else
						f:write("--\tlocal "..v1.." = "..name.."[0]."..v[1].."."..gType[v[3]].."();\r");
					end
				else
					if v[2] > 1 then
						f:write("--\tlocal "..v1.." = "..name.."."..v[1].."[0]."..gType[v[3]].."();\r");
					else
						f:write("\tlocal "..v1.." = "..name.."."..v[1].."."..gType[v[3]].."();\r");
					end
				end
			end
		end
		return true;
	end
	return false;
end

function RspToLua()
	local f = io.open("rsp.lua", "w");
	for name, tb in pairs(proto) do
		if string.sub(name,1,6) == "MsgRsp" then
			gNames = {}
			f:write("function on"..name.."( a"..name..", req )\n");
			f:write("\tlocal rsp = UiVar.new(a"..name..");\n");
			RspWriteStruct(f, "rsp", name, nil, 0);
			f:write("\tgClient:stopTip();\nend\n");
		end
	end
	f:close();
end

ParseProtoFile("GameMessage.proto");
ProtoToJs();
tagCmdToXml("GameMessageTags.h");
tagCmdToLua("GameMessageTags.h");