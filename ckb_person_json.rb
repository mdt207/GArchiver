#!/usr/bin/ruby
require 'pg'
require 'date'

source_temp = File.read("test.txt")
source_temp_lines = File.readlines("test.txt")
#puts source_temp_lines.length
#out_data_in_json_file = File.new("out_data_in_json.txt", "w")

prep_temp = source_temp

c = 0
a = 0
amount_of_files = 0
data_for_prep_temp = []
#puts prep_temp

def analyze(str)
 
 #str = str.sub(/\s$/, '')
 #str.scan(/\d+/) do |num|
  #res = num.to_i + 1
  #puts str + " | " + res.to_s 
 #end
 #puts str
 str.scan(/\#\d+[a-zA-ZČŬËŽâčŭëžь\+\-\s\d,]+/) do |info_to_put|
  info_to_put = info_to_put.sub(/^\s$/, '')
  str = info_to_put.to_s
  return str
  #puts str
 end
 #puts str
end

begin

    #json_data = '{"firstName": "Jamshid", "midName": "Unknown", "lastName": "Saydullayev", "martialStatus": 3, "age": 38,"address": {"state": "UZ", "city": "Tashkent" }, "phoneNumbers": [{"type": "home", "number": "121-334-196-913"},{"type": "mobile", "number": "+998 94 486-32-19"}], "nation": "Uzbek"}'

    con = PG.connect :dbname => 'ckbdb', :user => 'ckb', :password => 'ckbmdt'
    
	seq_of_files = File.readlines("list.txt")
	#puts seq_of_files.length
	#puts seq_of_files[0]

	while(amount_of_files < seq_of_files.length)
		str = seq_of_files[amount_of_files].to_s
		str = str.sub(/^\s/, '')
		str = str.sub(/\s$/, '')
		File.open(File.dirname(__FILE__) + '/' + str.to_s, "r:UTF-8").each{ |line| data_for_prep_temp << analyze(line) }
		#puts data_for_prep_temp.length


		while(c < data_for_prep_temp.length)
		#while(c < source_temp_lines.length)
			a = c + 1
			#puts data_for_prep_temp[c].to_s
	 
			data_for_prep_temp[c].scan(/\#\d{1,2}/) do |tag_num|
				tag_num = tag_num.sub(/^\s$/, '')
				tag_num = tag_num.sub(/\s$/, '')
				tag_num = tag_num.sub(/\#/, '')
			#puts tag_num
	   
			if a.to_i == tag_num.to_i
				#puts a.to_s + " - " + data_for_prep_temp[c]
				str = data_for_prep_temp[c].sub(/\#\d+/,'')
				#str = str.gsub(/\s/,'');
				str = str.sub(/^\s/,'');
				str = str.sub(/\s$/,'');
				#puts str
				prep_temp = prep_temp.sub(/\#\d+/, str)
			end
	 
			#if a.to_i != data_for_prep_temp[c][1].to_i
			if a.to_i != tag_num.to_i
			#puts a.to_s + " " + data_for_prep_temp[c][1].to_s
			
				if a.to_i == 4 #"martial status"
					prep_temp = prep_temp.sub('#'+a.to_s, '-1')	
				end
				
				if a.to_i == 5 #"age"
				    age = n = 0 
					puts "age: "
					while(n < data_for_prep_temp.length) 
						data_for_prep_temp[n].scan(/\d{4}/) do |year_of_birth|
						age = Date.today.year.to_i - year_of_birth.to_i
						puts age
						end
						n += 1
					end
					if age != 0 
						prep_temp = prep_temp.sub('#'+a.to_s, age.to_s)	
					end 	
				end
				
				if a.to_i == 6 #"birth year"
				    year = n = 0 
					puts "birth year: "
					while(n < data_for_prep_temp.length) 
						data_for_prep_temp[n].scan(/\#5\s\d{2}/) do |age|
						
						age = age.sub(/\#5\s/,'')
						year = Date.today.year.to_i - age.to_i
						puts year
						end
						n += 1
					end
					if year != 0 
						prep_temp = prep_temp.sub('#'+a.to_s, year.to_s)	
					end 	
				end		
				#puts a.to_s + " - " + data_for_prep_temp[c].to_s
				
				str = data_for_prep_temp[c].sub(/\#\d+/,'')
				str = str.sub(/^\s/,'');
				str = str.sub(/\s$/,'');
	  
				prep_temp = prep_temp.sub('#'+tag_num.to_s, str)
				prep_temp = prep_temp.sub('#'+a.to_s, '0') # put space unless found in the data_for_prep_temp
				#prep_temp = prep_temp.sub('#'+data_for_prep_temp[c][1].to_s, str) 
			end
	  
			end 
			c += 1
		end #while( c < )
		# back to the initial state
		c = 0
		while(c < source_temp_lines.length)
		#puts prep_temp
		 prep_temp.scan(/\#\d{1,2}/) do |tag_num|
		  #puts tag_num
		  prep_temp = prep_temp.sub(tag_num.to_s, '0')
		 end
		 c+=1
		end
		puts prep_temp
		
		#out_data_in_json_file.puts prep_temp
		res = con.exec_params("insert into ckb_persons (ckb_person) values ($1)", [prep_temp])
		
		prep_temp = source_temp
		c = 0
		data_for_prep_temp = []
		
		#next file in the turn
		amount_of_files += 1
	end # list of file

rescue PG::Error => e

    puts e.message

ensure

    con.close if con

end
#puts prep_temp

#out_data_in_json_file.close
