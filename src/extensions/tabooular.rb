require 'json'
require 'open3'
require 'base64'

module ReVIEW
	class Compiler
		defblock :tabletabooularraw, 0..2
		defblock :tabletabooularwideraw, 0..2
	end

	class Builder
		protected
		def tabooular_to(format, lines)
			def compile(row)
				row.each do | col |
					next unless col['type'] === 'cell'
					col['data'] = compile_inline(col['data'])
				end
			end

			plain = Base64.decode64(lines.join(''))
			stdout, stderr, status = Open3.capture3(
				"tabooular -if plain -of json",
				:stdin_data => plain
			)

			json = JSON.parse(stdout)
			(json['head'] || []).each{ |row| compile(row) }
			json['body'].each do | rows |
				rows.each{ |row| compile(row) }
			end

			stdout, stderr, status = Open3.capture3(
				"tabooular -if json -of #{format}",
				:stdin_data => JSON.generate(json)
			)

			return stdout
		end
	end

	class LATEXBuilder
		def tabletabooularraw(lines, id, caption, env='tabooular')
			begin
				puts '\begin{table}[!htbp]'
				puts '\centering' if env === 'tabooular'
				puts '\begin{'+env+'}'
				puts '\centering' if env === 'tabooular*'
				puts macro('reviewtablecaption', compile_inline(caption))
				puts macro('label', table_label(id))
				puts tabooular_to('latex', lines)
				puts '\end{'+env+'}'
				puts '\end{table}'
				blank
			rescue KeyError
				error "no such table: #{id}"
			end
		end

		def tabletabooularwideraw(lines, id, caption)
			tabletabooularraw(lines, id, caption, 'tabooular*')
		end
	end

	class HTMLBuilder
		def tabletabooularraw(lines, id, caption)
			begin
				puts %Q[<div id="#{normalize_id(id)}" class="table">]
				table_header(id, caption)
				puts tabooular_to('html', lines)
				puts %Q[</div>]
			rescue KeyError
				error "no such table: #{id}"
			end
		end

		def tabletabooularwideraw(lines, id, caption)
			tabletabooularraw(lines, id, caption)
		end
	end
end
